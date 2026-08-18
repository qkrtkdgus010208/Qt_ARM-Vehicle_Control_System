#include "device_driver.h"

void SysTick_Run(unsigned int msec)
{
	SysTick->CTRL = (0<<2)+(0<<1)+(0<<0);
	SysTick->LOAD = (unsigned int)((HCLK/(8.*1000.))*msec+0.5);
	SysTick->VAL = 0;
	Macro_Set_Bit(SysTick->CTRL, 0);
}

int SysTick_Check_Timeout(void)
{
	return ((SysTick->CTRL >> 16) & 0x1);
}

unsigned int SysTick_Get_Time(void)
{
	return SysTick->VAL;
}

unsigned int SysTick_Get_Load_Time(void)
{
	return SysTick->LOAD;
}

void SysTick_Stop(void)
{
	SysTick->CTRL = 0<<0;
}

// 마이크로초(us) 단위 딜레이
void SysTick_Delay_Us(unsigned int usec)
{
    // SysTick 정지 및 설정: CLKSOURCE = 0 (HCLK/8 = 12MHz), 인터럽트 OFF(0), Counter OFF(0)
    SysTick->CTRL = 0;
    
    // 96MHz / 8 = 12MHz 이므로 1us당 12 카운트
    // (usec * 12) 계산 (최대 24비트 = 약 1.39초까지 가능)
    SysTick->LOAD = (usec * (96000000UL / 8000000UL)) - 1; 
    SysTick->VAL = 0; // 카운터 초기화
    
    // SysTick 시작 (ENABLE bit = 1)
    SysTick->CTRL = (0 << 2) | (0 << 1) | (1 << 0);
    
    // COUNTFLAG (bit 16)가 1이 될 때까지(다운카운트가 0에 도달할 때까지) 대기
    while (!((SysTick->CTRL >> 16) & 0x1));
    
    // SysTick 정지
    SysTick->CTRL = 0;
}

// 밀리초(ms) 단위 딜레이
void SysTick_Delay_Ms(unsigned int msec)
{
    while (msec--)
    {
        SysTick_Delay_Us(1000); // 1000us = 1ms씩 반복 호출
    }
}
