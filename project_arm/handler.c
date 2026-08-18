#include "device_driver.h"
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT 1000

void Handler(void)
{
    Key_Handler();
    UART_Handler();
    Timer_Handler();
}

void Key_Handler(void)
{
    // static unsigned char stop = 0;

    // if (Key_Pressed)
    // {
    //     Key_Pressed = 0;
    //     TIM4_Interrupt_Enable(1, 1, TIMEOUT);
    // }
    // else if (Key_Released)
    // {
    //     Key_Released = 0;
    //     TIM4_Stop();

    //     if (stop)
    //     {
    //         stop = 0;
    //         return;
    //     }

    //     cur_motor_state = (cur_motor_state == CW) ? CCW : CW;
    // }

    // if (TIM4_Expired)
    // {
    //     TIM4_Expired = 0;
    //     stop = 1;
    //     cur_motor_state = STOP;
    // }
}

void UART_Handler(void)
{
    if (!Uart_Data_In)
        return;

    Uart_Data_In = 0;

    char header = rx_buf[0];      // 첫 글자 식별자 ('A', 'S' 등)
    int value = atoi(&rx_buf[1]); // 뒤의 숫자 문자열을 정수로 변환

    switch (header)
    {
    case 'D': // 방향 제어 ('F', 'R', 'S')
        if (rx_buf[1] == 'F')
            cur_motor_state = CW;
        else if (rx_buf[1] == 'R')
            cur_motor_state = CCW;
        else if (rx_buf[1] == 'S')
            cur_motor_state = STOP;
        break;

    case 'S': // DC 모터 속도 명령 (0 ~ 200)
        cur_motor_speed = value;
        break;

    case 'A': // 입력 범위 제한 (0 ~ 180도)
        if (value < 0)
            value = 0;
        if (value > 180)
            value = 180;
        // 0도 -> 35, 90도 -> 85, 180도 -> 135 변환 공식
        cur_servo_motor_speed = 35 + ((value * 100) / 180);
        break;

    default:
        break;
    }
}

void Timer_Handler()
{
    if (TIM4_Expired)
    {
        TIM4_Expired = 0;
        DHT11_Controller();
    }
}
