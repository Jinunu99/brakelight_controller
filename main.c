/*
컴파일 방법 : make
실행파일 삭제 : make clean
*/
#include "./include/adc.h"
#include "./include/led.h"
#include "./include/motor.h"
#include "./include/uart.h"


int hw_INIT(int *spi_handle, int *uart_fd)
{
    // GPIO 초기화
    if (gpioInitialise() < 0)
    {
        printf("pigpio 초기화 실패!\n");
        return -1;
    }

    // SPI 초기화
    *spi_handle = spiOpen(SPI_CHANNEL, SPI_SPEED, 0);
    if (*spi_handle < 0)
    {
        printf("SPI 오픈 실패!\n");
        gpioTerminate();
        return -2;
    }

    // UART 초기화
    *uart_fd = uart_open(SERIAL_PORT);
    if (*uart_fd < 0)
    {
        printf("UART 오픈 실패!\n");
        spiClose(*spi_handle);
        gpioTerminate();
        return -3;
    }

    // LED 초기화
    if (led_INIT() != 0)
    {
        fprintf(stderr, "LED 초기화 실패!\n");
        spiClose(*spi_handle);
        gpioTerminate();
        close(*uart_fd);
        return -4;
    }

    return 0; // 성공
}

int main()
{
    // 하드웨어 초기화
    int spi_handle, uart_fd;
    if (hw_INIT(&spi_handle, &uart_fd) != 0)
        return 1;

    motor_INIT(); // 모터 정방향으로 초기화

    int control_pwm = 0; // 실제로 모터 제어할 pwm
    float PrevMotorSpeed = 0.0f; // 이전 모터 속도
    float MotorSpeed = 0.0f;     // 현재 모터 속도[m/s]
    float acceleration = 0.0f;   // 현재 가속도
    int led_flag = -1;           // LED를 노란색 -> 주황색으로 자연스럽게 바꾸기 위함

    struct timespec prevTime, currTime;
    clock_gettime(CLOCK_MONOTONIC, &prevTime);

    while (1)
    {
        int adc_brake = read_adc(spi_handle, 0); // 브레이크
        int adc_accel = read_adc(spi_handle, 1); // 엑셀
        int adc_step = read_adc(spi_handle, 2);  // 가변저항 (회생제동 단계)
        int step = step_set(adc_step);           // 회생제동 단계 설정

        // (550으로 나눈 이유는 압력센서를 쎄게 눌러도 약 550까지 올라감)
        int curr_pwm = (adc_accel * (255 - 55) / 550) + 55;   // 현재 pwm (55 ~ 255)

        // UART 수신
        char ch;
        char line[128];
        int idx = 0;
        while (read(uart_fd, &ch, 1) == 1)
        {
            if (ch == '\n')
            {
                line[idx] = '\0';
                float rx_MotorSpeed;
                if (sscanf(line, "MotorSpeed: %f", &rx_MotorSpeed) == 1)
                {
                    MotorSpeed = rx_MotorSpeed * 150; // 수신은 cm/s인데 실제를 가장해서 150을 곱함
                }
                idx = 0;
                break;
            }
            else if (idx < sizeof(line) - 1) // 수신 버퍼 오버플로우 확인
            {
                line[idx++] = ch;
            }
        }

        // 모터 가속도 계산
        clock_gettime(CLOCK_MONOTONIC, &currTime);
        double deltaT = (currTime.tv_sec - prevTime.tv_sec) + (currTime.tv_nsec - prevTime.tv_nsec) / 1e9;
        acceleration = (MotorSpeed - PrevMotorSpeed) / deltaT;
        PrevMotorSpeed = MotorSpeed;
        prevTime = currTime;

        // 페달을 밟음 (현재 pwm이 클때)
        if (curr_pwm > control_pwm)
        {
            control_pwm = curr_pwm; // 현재의 pwm으로 모터 제어

            led_flag = -1;
            led_set(COLOR(0, 0, 0)); // LED OFF
        }
        // 페달을 살짝 밟음 or 페달을 밟지 않을때
        else
        {   // 회생제동 (단계에 따른 감소)
            int brake_step = dowm_step_dynamic(step, control_pwm);
            control_pwm -= brake_step;
            
            if (control_pwm < PWM_MIN) // 최저 pwm인 55보다 작으면
                control_pwm = PWM_MIN;

            
            // 감속도가 0.0f는 그냥 무시
            if (acceleration == 0.0f)
                continue;

            // 감속도가 작으면 LED OFF
            if (acceleration > -7.0f)
            {
                led_flag = -1;
                led_set(COLOR(0, 0, 0)); // LED OFF
            }
            // 감속도가 크다면 LED를 노란색 -> 주황색으로 그라데이션으로 변화될 수 있도록
            else
            {
                // 페달을 뗀 직후의 pwm값을 led_flag에 넣으며 LED가 노랑 -> 주황으로 자연스럽게 변화되도록
                if (led_flag == -1)
                    led_flag = control_pwm;

                // 노란색 (255, 255, 0) -> 주황색 (255, 165, 0)
                // 여기서 90.0은 노란색과 주황색의 차이인데 주황색을 진하게 하기위해 150으로 설정
                int g = (int)(255.0f - ((float)(led_flag - control_pwm) / (float)(led_flag - PWM_MIN + 1) * 150.0f));
                led_set(COLOR(255, (uint8_t)g, 0));
            }
        }

        // 브레이크 페달을 밟으면 정지
        if (adc_brake > 10)
        {
            control_pwm = 0;
            led_set(COLOR(255, 0, 0));
        }

        // 모터 제어
        motor_PWM(control_pwm);


        printf("step: %d| 압력센서: %4d | PWM: %3d | MotorSpeed: %.3f[m/s] | acceleration: %.3f[m/s^2]\n", step, adc_accel, control_pwm, MotorSpeed, acceleration);
        fflush(stdout);

        gpioDelay(10000); // 10ms
    }

    led_finish();
    spiClose(spi_handle);
    gpioTerminate();
    close(uart_fd);

    return 0;
}
