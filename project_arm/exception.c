#include "device_driver.h"
#include <stdio.h>

void _Invalid_ISR(void)
{
	unsigned int r = Macro_Extract_Area(SCB->ICSR, 0x1ff, 0);
	printf("\nInvalid_Exception: %d!\n", r);
	printf("Invalid_ISR: %d!\n", r - 16);
	for (;;);
}

void EXTI15_10_IRQHandler(void)
{
	if (Key_Get_Pressed())
	{
		Key_Pressed = 1;
	}
	else
	{
		Key_Released = 1;
	}

	EXTI->PR = 0x1 << 13;
	NVIC_ClearPendingIRQ(40);
}

void USART2_IRQHandler(void)
{
	Uart_Data_In = 1;
	Uart_Data = (unsigned char)USART2->DR;
	NVIC_ClearPendingIRQ(38);
}

void TIM4_IRQHandler(void)
{
	// TIM4 Interrupt Pending Clear
	// NVIC Pending Clear
	Macro_Clear_Bit(TIM4->SR, 0);
	NVIC_ClearPendingIRQ(30);

	TIM4_Expired = 1;
}