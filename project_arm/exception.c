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
	char c = (unsigned char)USART2->DR;

	if (c == '\n')
        {
            rx_buf[rx_idx] = '\0'; // 문자열 끝 완성 ("A90\0")
            rx_idx = 0;
            Uart_Data_In = 1; // 메인 루프에 패킷 수신 완료 알림
        }
        else if (c != '\r') // 캐리지 리턴 문자 무시
        {
            if (rx_idx < sizeof(rx_buf) - 1)
            {
                rx_buf[rx_idx++] = c;
            }
        }

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