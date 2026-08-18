#include "device_driver.h"
#include <stdio.h>

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
    if (!Uart_Data_In) return;

    Uart_Data_In = 0;
    char c = Uart_Data;

    // if (c == 'S')
    // {
    //     cur_motor_state = STOP;
    //     printf("%c\n", c);
    // }
    // else if (c == 'F')
    // {
    //     cur_motor_state = CW;
    //     printf("%c\n", c);
    // }
    // else if (c == 'R')
    // {
    //     cur_motor_state = CCW;
    //     printf("%c\n", c);
    // }
    // else if (c >= '1' && c <= '9')
    // {
    //     cur_motor_speed = c - '0';
    //     printf("%c\n", c);
    // }

    if (c == '0')
    {
        LED_Off();
    }
    else if (c == '1')
    {
        LED_On();
    }
}

void Timer_Handler()
{
    if (TIM4_Expired)
    {
        TIM4_Expired = 0;
        dht11_main();
    }
}
