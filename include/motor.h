#ifndef MOTOR_H
#define MOTOR_H

#include "def.h"
#include <math.h> // lroundf 함수를 위함
#include <time.h> // clock_gettime을 위함

// 파라미터
#define PWM_MIN 55
#define PWM_MAX 255
#define STEP_MIN 1
#define STEP_MAX 20

// 회생제동 단계별 기본감속
#define BASE_DOWN_L1  3   // 약함
#define BASE_DOWN_L2  4   // 보통
#define BASE_DOWN_L3  5   // 강함

// PWM 가중 계수
#define K_PWM_L1 0.6f
#define K_PWM_L2 1.0f
#define K_PWM_L3 1.4f

// 곡률(>=1). 1이면 선형, 2면 제곱, 1.5면 약한 곡선
#define GAMMA_L1 1.4f
#define GAMMA_L2 1.5f
#define GAMMA_L3 1.6f

// 왼쪽 모터
#define M1_IN1 17
#define M1_IN2 27
#define M1_ENA 12

// 오른쪽 모터
#define M2_IN3 22
#define M2_IN4 23
#define M2_ENB 13

static inline float clampf(float x, float a, float b);
int dowm_step_dynamic(int level, int control_pwm);


void motor_INIT(); // 모터 초기화
void motor_PWM(int pwm); // 모터 속도 제어


#endif