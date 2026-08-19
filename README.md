# STM32 & Qt 기반 차량 전장 모니터링 시스템

<img width="1522" height="848" alt="image" src="https://github.com/user-attachments/assets/b6774df4-95da-4b87-a571-8b1e6f13cc96" />

## 프로젝트 개요

### 🚗 프로젝트 소개

> QT GUI와 Cortex-M4를 UART로 연동하여 차량의 주행과 조향을 원격 제어하고, 카메라 및 차량 내부 환경 정보를 실시간 모니터링하는 스마트 시스템

| 항목 | 내용 |
| --- | --- |
| **프로젝트 명** | STM32 & Qt 기반 차량 전장 모니터링 시스템  |
| **개발 인원** | 4명 |
| **개발 언어** | C / C++ |
| **MCU** | CORTEX-M4 |
| **GUI** | QT |
| **통신** | UART Serial communication |
| **센서** | DHT11 온습도 센서 |
| **Actuator** | DC Motor / Servo Motor / LED |
| **영상** | Camera + QT |

### 팀 구성

- 박상현
- 고지현
- 곽혜민
- 김경태

---

## 주요 기능

-	차량 ON/OFF 제어
-	차량 카메라 영상 표시 
-	전진 / 중립/ 후진 기어 제어
-	차량 속도 제어 (Accelator 구현)
-	좌 / 우 조향 및 방향지시등 제어
-	비상등 제어
-	차량 온도 및 습도 표시 
-	속도 및 주행 시간 표시

---

## 시스템 요구 사항
<img width="416" height="824" alt="image" src="https://github.com/user-attachments/assets/e0da80e4-e0c6-4750-aabb-3cda305885bf" />

---

## 기능 요구사항

**차량 제어**
-	ON 버튼을 누르면 차량 제어 시스템을 활성화한다. 
-	OFF 버튼을 누르면 차량을 정지 상태로 전환한다. 
-	F (전진) / N (중립) / B(후진) 선택시 해당 모드로 전환한다.
-	Accelator를 통해 차량 Motor PWM Duty를 변경한다. 
-	좌/우 조향 명령을 통해 서보 모터의 각도를 제어한다. 

**전장 제어**
-	LED L/R 버튼을 통해 좌/우 방향지시등을 제어한다. 
-	비상등 버튼을 통해 좌/우 방향지시등을 동시에 제어한다.

**모니터링**
-	카메라 영상을 Qt화면에 표시한다. 
-	현재 차량 속도를 Qt 화면 및 터미널에 표시한다. 
-	온도·습도 정보를 Qt 화면 및 터미널에 표시한다. 
-	주행 시간을 Qt 화면 및 터미널에 표시한다. 
-	차량 OFF시 주행 기록을 Qt Dialog로 표시한다. 

**통신**
-	Qt 에서 차량 제어 명령을 UART로 STM32에 전달한다. 
-	STM32에서 차량 상태 정보를 UART로 Qt에 전달한다.

## UI 구성

### 화면 구성
<img width="1521" height="848" alt="image" src="https://github.com/user-attachments/assets/370b9d21-ea4d-4c14-87c4-45556860bdfa" />

### 위젯 구성

|화면 요소|	Qt Widget	|용도|
|---|---|---|
|Camera / Temp / Humid / Speed |	QLabel|	Display|
|ON / OFF|	QPushButton|	차량 활성화 / 정지|
|LED L / R, , 비상등|	QPushButton	|방향 지시등 제어/ 비상등|
|F / N / B |	QPushButton (Checkable)	모드 변경 |
|Accelerator|	QSlider|	Motor PWM 조절|
|Handle , L / R , Speed	|QDial , QPushButton|	핸들 표시, 조향 제어|

### Dialog
>  주행 종료시 해당 주행의 정보를 확인 할 수 있는 **Drive Log Dialog**를 표시
<img width="635" height="909" alt="image" src="https://github.com/user-attachments/assets/9a0e1c6c-d6bf-4b03-92ff-a917c0c9964f" />

|항목|설명|
|---|---|
|**startTime**|주행을 시작한 시각|
|**endTime**|주행을 종료한 시각|
|**avgSpeed**|평균 속도|
|**maxSpeed**|최고 속도|
|**driveTime**| 총 주행 시간|
|**leftCount**| 좌회전 횟수|
|**rightCount**| 우회전 횟수|
|**temp**| 차량 내부 온도|
|**humidity**|차량 내부 습도|

### Message Box
<img width="294" height="110" alt="스크린샷 2026-08-19 103727" src="https://github.com/user-attachments/assets/b2b7fd46-5172-45e6-a08e-ed6374ab915e" />

> 사용자의 안전을 위해 차량의 속도가 0이상일 때 Drive-> Reverse로 모드를 변경하거나 Reverse -> Drive로 모드를 변경할 때 경고
 
<img width="209" height="119" alt="image" src="https://github.com/user-attachments/assets/6393c681-3800-4629-82cc-2694190ad704" />

> 차량 주행 종료 전 사용자의 의도를 한번 더 확인

---

## 동작 영상

https://github.com/user-attachments/assets/e23b8202-c126-4254-a741-647ff3460128

---

## 하드웨어 구성

<img width="802" height="410" alt="스크린샷 2026-08-19 114632" src="https://github.com/user-attachments/assets/2df6c8a5-962d-41ac-b271-7b786aeac66c" />
