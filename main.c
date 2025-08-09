/*
컴파일 방법 : make
실행파일 삭제 : make clean
*/
#include "./include/adc.h"
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
    float MotorSpeed = 0.0f; // 모터의 속도[m/s]

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
                    MotorSpeed = rx_MotorSpeed * 150; // 수신은 cm/s인데 실제를 가장해서 170을 곱함
                }
                idx = 0;
                break;
            }
            else if (idx < sizeof(line) - 1) // 수신 버퍼 오버플로우 확인
            {
                line[idx++] = ch;
            }
        }

        // 페달을 밟음 (현재 pwm이 클때)
        if (curr_pwm > control_pwm)
        {
            control_pwm = curr_pwm; // 현재의 pwm으로 모터 제어
        }
        // 페달을 살짝 밟음 or 페달을 밟지 않을때
        else
        {   // 회생제동 (단계에 따른 감소)
            int brake_step = dowm_step_dynamic(step, control_pwm);
            control_pwm -= brake_step;
            
            if (control_pwm < PWM_MIN)
                control_pwm = 0;
        }

        // 브레이크 페달을 밟으면 정지
        if (adc_brake > 10)
            control_pwm = 0;

        // 모터 제어
        motor_PWM(control_pwm);


        printf("step: %d| 압력센서: %4d | PWM: %3d | MotorSpeed: %.3f[m/s]\n", step, adc_accel, control_pwm, MotorSpeed);
        fflush(stdout);



        gpioDelay(10000); // 10ms
    }

    spiClose(spi_handle);
    gpioTerminate();
    close(uart_fd);
    return 0;
}
