# 운전자 가시성 향상 브레이크등 제어기 설계

## 기술 스택
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)  
![Raspberry Pi](https://img.shields.io/badge/-Raspberry_Pi-C51A4A?style=for-the-badge&logo=Raspberry-Pi) ![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)

## 개요
감속 상황을 후방운전자가 인지할 수 있도록 개선한 브레이크등 제어기,  
원페달 드라이빙 중 가속 페달을 완전히 떼지 않아도 감속에 따라 브레이크등이 점등될 수 있도록 설계  

  
* 개발 기간 : 2024.03.04 ~ 06.07
* 프로젝트 인원 : 1명

## 프로젝트 핵심기능
`Encoder Motor (아두이노)`
* 4체배 카운터로 엔코더 펄스를 측정하여 실제 모터 속도를 계산
* 계산 방법 : 초당 펄스수 (측정) -> 초당 회전수(RPS) -> 분당 회전수(RPM) -> 바퀴 속도(cm/s)
* UART 통신으로 바퀴 속도 값 송신

`Control (라즈베리파이)`
* 압력 센서 2개(엑셀, 브레이크), 가변 저항(회생제동 단계)
* 회생제동을 1 ~ 3단계로 설정 가능 (1단계: 약한 감속, 2단계: 중간 감속, 3단계: 강한 감속)
* 엑셀에서 발을 떼면 회생제동 단계에 따라 모터 감속량을 다르게 적용
* 7m/s²이상의 감속도 구간에서 점진적으로 색 변화 발생 (노란색 -> 주황색)
* 여기서 감속도 계산은 아두이노로부터 수신된 바퀴 속도로 계산 적용

<img width="2244" height="1032" alt="Image" src="https://github.com/user-attachments/assets/ff45b362-f606-427b-ada2-fa90ed01313f" />
[회생제동 단계에 따른 모터 감속량 그래프]


## 시연 영상
|기본 감속|감속 -> 가속 -> 재감속|감속 -> 정지|
|---|---|---|
|![Image](https://github.com/user-attachments/assets/f8518989-411c-4241-9dac-786d7aa34fe8)|![Image](https://github.com/user-attachments/assets/dc9e377f-181e-4ddd-b192-0cceccce6547)|![Image](https://github.com/user-attachments/assets/3a50ae4c-220f-4192-83da-f314cb0f6956)|
