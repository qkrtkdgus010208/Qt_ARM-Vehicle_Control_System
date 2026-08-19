#include "device_driver.h"

void LED_Init(void)
{
	/* 아래 코드 수정 금지 : Port-A Clock Enable */
	Macro_Set_Bit(RCC->AHB1ENR, 0); 

	// LED를 출력으로 설정하고 초기 OFF
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, 10);
	Macro_Clear_Bit(GPIOA->OTYPER, 5);
	Macro_Clear_Bit(GPIOA->ODR, 5);

	// 깜빡이용 Port-C Clock Enable 및 초기 설정
	Macro_Set_Bit(RCC->AHB1ENR, 2);

	Macro_Write_Block(GPIOC->MODER, 0x3, 0x1, 10U); // PC5: MODER5 (bit 11:10)
    Macro_Write_Block(GPIOC->MODER, 0x3, 0x1, 12U); // PC6: MODER6 (bit 13:12)
    Macro_Write_Block(GPIOC->MODER, 0x3, 0x1, 16U); // PC8: MODER8 (bit 17:16)

    // 출력 타입 Push-Pull(0) 설정
    Macro_Clear_Bit(GPIOC->OTYPER, 5);
    Macro_Clear_Bit(GPIOC->OTYPER, 6);
    Macro_Clear_Bit(GPIOC->OTYPER, 8);

    // 초기 상태: 모두 소등 (ODR 초기화)
    Macro_Clear_Bit(GPIOC->ODR, 5);
    Macro_Clear_Bit(GPIOC->ODR, 6);
    Macro_Clear_Bit(GPIOC->ODR, 8);
}

void LED_On(void)
{
	// LED On
	Macro_Set_Bit(GPIOA->ODR, 5); 
}

void LED_Off(void)
{
	// LED Off
	Macro_Clear_Bit(GPIOA->ODR, 5);
}

void LED_Left_Toggle(void)
{
	Macro_Invert_Bit(GPIOC->ODR, 5);
	Macro_Clear_Bit(GPIOC->ODR, 6);
    Macro_Clear_Bit(GPIOC->ODR, 8);
}

void LED_Right_Toggle(void)
{
	Macro_Clear_Bit(GPIOC->ODR, 5);
	Macro_Invert_Bit(GPIOC->ODR, 6);
	Macro_Clear_Bit(GPIOC->ODR, 8);
}

void LED_Emergency_Toggle(void)
{
	Macro_Clear_Bit(GPIOC->ODR, 5);
    Macro_Clear_Bit(GPIOC->ODR, 6);
	Macro_Invert_Bit(GPIOC->ODR, 8);
}

void LED_All_Off(void)
{
    Macro_Clear_Bit(GPIOC->ODR, 5);
    Macro_Clear_Bit(GPIOC->ODR, 6);
    Macro_Clear_Bit(GPIOC->ODR, 8);
}