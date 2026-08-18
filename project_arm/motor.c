#include "device_driver.h"
#include <stdio.h>

#define DELAY_TIME 1000

#if 0
void MOTOR_Init(void)
{
	/* 아래 코드 수정 금지 : Port-A Clock Enable */
	Macro_Set_Bit(RCC->AHB1ENR, 0U); 

	// MOTOR를 출력으로 설정하고 초기 OFF
	Macro_Write_Block(GPIOA->MODER, 0xF, 0x5, 0U);
	Macro_Clear_Area(GPIOA->OTYPER, 0x3, 0U);
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0U);
}

void MOTOR_Stop(void)
{
    Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0U);
    TIM2_Delay(DELAY_TIME);
}

void MOTOR_CW(void)
{
    if (prev_motor_state != STOP) MOTOR_Stop();
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x2, 0U);
}

void MOTOR_CCW(void)
{
    if (prev_motor_state != STOP) MOTOR_Stop();
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x1, 0U);
}
#endif

#if 1
void MOTOR_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 0U);

	Macro_Write_Block(GPIOA->MODER, 0xF, 0x5, 0U);
	Macro_Clear_Area(GPIOA->OTYPER, 0x3, 0U);
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0U);

	TIM5_Out_PWM_Generation(0);
}

void MOTOR_Stop(void)
{
	Macro_Write_Block(GPIOA->MODER, 0xF, 0x5, 0U);
	Macro_Clear_Area(GPIOA->OTYPER, 0x3, 0U);
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0U);

    TIM2_Delay(DELAY_TIME);
}

void MOTOR_CW(void)
{
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, 0U);
	Macro_Clear_Bit(GPIOA->OTYPER, 0U);
	Macro_Clear_Bit(GPIOA->ODR, 0U);

	Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, 2U);
	Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x2, 4U);
}

void MOTOR_CCW(void)
{
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, 0U);
	Macro_Write_Block(GPIOA->AFR[0], 0xf, 0x2, 0U);

	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, 2U);
	Macro_Clear_Bit(GPIOA->OTYPER, 1U);
	Macro_Clear_Bit(GPIOA->ODR, 1U);
}
#endif
