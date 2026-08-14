#include "device_driver.h"
#include <stdio.h>

void State_Management(void)
{
    Motor_State_Management();
}

void Motor_State_Management(void)
{
    if (prev_motor_state != cur_motor_state)
    {
        Motor_Controller();
        prev_motor_state = cur_motor_state;
    }

    if (prev_motor_speed != cur_motor_speed)
    {
        TIM5_Out_PWM_Generation(cur_motor_speed);
        prev_motor_speed = cur_motor_speed;
    }
}