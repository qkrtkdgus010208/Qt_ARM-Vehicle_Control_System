#include "device_driver.h"
#include <stdio.h>

Motor_State_t prev_motor_state = STOP;
Motor_State_t cur_motor_state = STOP;
unsigned char prev_motor_speed = 1;
unsigned char cur_motor_speed = 1;

volatile unsigned char Key_Pressed = 0;
volatile unsigned char Key_Released = 0;
volatile unsigned char Uart_Data_In = 0;
volatile unsigned char Uart_Data = 0;
volatile unsigned char TIM4_Expired = 0;

static void Sys_Init(int baud) 
{
	SCB->CPACR |= (0x3 << 10*2)|(0x3 << 11*2); 
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
	LED_Init();
}

#if 1
void Main(void)
{
	Sys_Init(115200);
	printf("\nMotor Test\n");

    Key_ISR_Enable(1);
	Uart2_RX_Interrupt_Enable(1);

    TIM5_Out_Init();
	MOTOR_Init();

	for(;;)
	{
        Handler();
		State_Management();
	}
}
#endif