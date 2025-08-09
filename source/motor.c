#include "../include/motor.h"

static inline float clampf(float x, float a, float b){ return x < a ? a : (x > b ? b : x); }

int dowm_step_dynamic(int level, int control_pwm)
{
    int base;    // 회생제동 단계별 기본 감속
    float k_pwm; // PWM 가중 계수
    float gamma; // 곡률

    switch(level)
    {
        case 1:
            base = BASE_DOWN_L1;
            k_pwm = K_PWM_L1;
            gamma = GAMMA_L1;
            break;
        case 2:
            base = BASE_DOWN_L2;
            k_pwm = K_PWM_L2;
            gamma = GAMMA_L2;
            break;
        case 3:
            base = BASE_DOWN_L3;
            k_pwm = K_PWM_L3;
            gamma = GAMMA_L3;
            break;
    }

    float p = clampf((float)control_pwm / (float)PWM_MAX, 0.f, 1.f);

    // 높은 PWM에서 더 세게 감속되도록 곡선 가중
    // step = base * (1 + k * p^gamma)
    float step_f = (float)base * (1.0f + k_pwm * powf(p, gamma));

    int step = (int)lroundf(clampf(step_f, STEP_MIN, STEP_MAX));
    return step;
}


void motor_INIT()
{
    gpioWrite(M1_IN1, 1);
    gpioWrite(M1_IN2, 0);
    gpioWrite(M2_IN3, 1);
    gpioWrite(M2_IN4, 0);
}

void motor_PWM(int pwm)
{
    gpioPWM(M1_ENA, pwm);
    gpioPWM(M2_ENB, pwm);
}