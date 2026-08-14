#include "device_driver.h"
#include <stdio.h>

void Motor_Controller(void)
{
    switch (cur_motor_state)
    {
    case STOP:
        MOTOR_Stop();
        break;

    case CW:
        if (prev_motor_state == STOP)
            MOTOR_CW();
        else
        {
            MOTOR_Stop();
            MOTOR_CW();
        }
        break;

    case CCW:
        if (prev_motor_state == STOP)
            MOTOR_CCW();
        else
        {
            MOTOR_Stop();
            MOTOR_CCW();
        }
        break;

    default:
        MOTOR_Stop();
        break;
    }
}