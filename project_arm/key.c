#include "device_driver.h"

void Key_Poll_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 2); 
	Macro_Write_Block(GPIOC->MODER, 0x3, 0x0, 26);
}

int Key_Get_Pressed(void)
{
	return Macro_Check_Bit_Clear(GPIOC->IDR, 13);	
}

void Key_Wait_Key_Pressed(void)
{
	while(!Macro_Check_Bit_Clear(GPIOC->IDR, 13));
}

void Key_Wait_Key_Released(void)
{
	while(!Macro_Check_Bit_Set(GPIOC->IDR, 13));
}

void Key_ISR_Enable(int en)
{
	if(en)
	{
		Macro_Set_Bit(RCC->AHB1ENR, 2); 
		Macro_Write_Block(GPIOC->MODER, 0x3, 0x0, 26);

		Macro_Set_Bit(RCC->APB2ENR, 14); 
		Macro_Write_Block(SYSCFG->EXTICR[3], 0xf, 0x2, 4);

		Macro_Set_Bit(EXTI->RTSR, 13);
		Macro_Set_Bit(EXTI->FTSR, 13);
		EXTI->PR = 0x1 << 13;
		
		NVIC_ClearPendingIRQ((IRQn_Type)40);
		Macro_Set_Bit(EXTI->IMR, 13);
		NVIC_EnableIRQ((IRQn_Type)40);
	}

	else
	{
		NVIC_DisableIRQ((IRQn_Type)40);
	}
}