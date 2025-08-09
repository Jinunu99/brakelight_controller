#include <Encoder.h>

#define PI 3.14159
#define RADIUS 0.035      // 반지름 : 3.5cm = 0.035m 
#define L_ENCODER_A_PIN 2
#define L_ENCODER_B_PIN 4
#define R_ENCODER_A_PIN 3
#define R_ENCODER_B_PIN 5

Encoder L_Encoder(L_ENCODER_A_PIN, L_ENCODER_B_PIN);
Encoder R_Encoder(R_ENCODER_A_PIN, R_ENCODER_B_PIN);

unsigned long last_time = 0; // 100ms마다 측정하기 위함
long currPosition[2] = {};  // 엔코더 현재 카운터 값
long prevPosition[2] = {};  // 엔코더 이전 카운터 값
long delta[2] = {};         // 0.1초 동안 엔코더 카운터 변화
float pps[2] = {};          // 엔코더의 초당 펄스수
float rps[2] = {};          // 초당 회전수
float rpm[2] = {};          // 분당 회전수
float speed[2] = {};        // 바퀴 속도


void setup()
{
  Serial.begin(115200);  // 라즈베리파이와 속도 일치(115200 권장)

  pinMode(L_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(L_ENCODER_B_PIN, INPUT_PULLUP);
  pinMode(R_ENCODER_A_PIN, INPUT_PULLUP);
  pinMode(R_ENCODER_B_PIN, INPUT_PULLUP);
}

void loop()
{
  // 100ms마다 측정/송신
  if (millis() - last_time >= 100)
  {
    last_time = millis();
    
    /* 왼쪽 엔코더 */
    currPosition[0] = (-1) * L_Encoder.read();           // 엔코더 카운터 값 읽기
    delta[0] = currPosition[0] - prevPosition[0]; // 0.1초 동안 카운터 변화
    prevPosition[0] = currPosition[0];            // 현재 카운터를 이전 카운터에 저장

    // 초당 펄스수 = (펄스 / 0.1초) × 10
    pps[0] = delta[0] * 10.0;

    // 초당 회전수 = pps / N(PPR : 엔코더 한 회전당 펄스수, 데이터시트에 11이라 나와있음)
    rps[0] = pps[0] / (11.0 * 4); // 여기서 4를 곱한 이유는 4체배 방식 측정이기 때문

    // 분당 회전수 = rps x 60
    rpm[0] = rps[0] * 60 / 21.3;

    // 바퀴 속도 = 바퀴의 원주 x rpm / 60
    speed[0] = 2 * PI * RADIUS * rpm[0] / 60;


    /* 오른쪽 엔코더 (왼쪽 엔코더와 계산 동일)*/
    currPosition[1] = R_Encoder.read();
    delta[1] = currPosition[1] - prevPosition[1];
    prevPosition[1] = currPosition[1];

    pps[1] = delta[1] * 10.0;
    rps[1] = pps[1] / (11.0 * 4);
    rpm[1] = rps[1] * 60 / 21.3;
    speed[1] = 2 * PI * RADIUS * rpm[1] / 60;

    
    // 시리얼 송신 (왼쪽, 오른쪽 모터 속도의 평균값 [cm/s])
    Serial.print("MotorSpeed: ");
    Serial.println((speed[0] + speed[1]) / 2, 3);  // 소수점 3자리까지
  }
}