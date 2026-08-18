#include "device_driver.h"
#include <stdio.h>

#define TIM2_TICK         	(20) 				// usec
#define TIM2_FREQ 	  		(1000000/TIM2_TICK)	// Hz
#define TIME2_PLS_OF_1ms  	(1000/TIM2_TICK)
#define TIM2_MAX	  		(0xffffu)

#define TIM4_TICK	  		(20) 				// usec
#define TIM4_FREQ 	  		(1000000/TIM4_TICK) // Hz
#define TIME4_PLS_OF_1ms  	(1000/TIM4_TICK)
#define TIM4_MAX	  		(0xffffu)

void TIM2_Stopwatch_Start(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->PSC = (unsigned int)(TIMXCLK/50000.0 + 0.5)-1;
	TIM2->ARR = TIM2_MAX;

	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Set_Bit(TIM2->CR1, 0);
}

unsigned int TIM2_Stopwatch_Stop(void)
{
	unsigned int time;

	Macro_Clear_Bit(TIM2->CR1, 0);
	time = (TIM2_MAX - TIM2->CNT) * TIM2_TICK;
	return time;
}

/* Delay Time Max = 65536 * 20use = 1.3sec */

#if 0

void TIM2_Delay(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
	TIM2->ARR = TIME2_PLS_OF_1ms * time;

	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Clear_Bit(TIM2->SR, 0);
	Macro_Set_Bit(TIM2->CR1, 0);

	while(Macro_Check_Bit_Clear(TIM2->SR, 0));

	Macro_Clear_Bit(TIM2->CR1, 0);
}

#else

/* Delay Time Extended */

void TIM2_Delay(int time)
{
	int i;
	unsigned int t = TIME2_PLS_OF_1ms * time;

	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5)-1;
	TIM2->CR1 = (1<<4)|(1<<3);
	TIM2->ARR = 0xffff;
	Macro_Set_Bit(TIM2->EGR,0);

	for(i=0; i<(t/0xffffu); i++)
	{
		Macro_Set_Bit(TIM2->EGR,0);
		Macro_Clear_Bit(TIM2->SR, 0);
		Macro_Set_Bit(TIM2->CR1, 0);
		while(Macro_Check_Bit_Clear(TIM2->SR, 0));
	}

	TIM2->ARR = t % 0xffffu;
	Macro_Set_Bit(TIM2->EGR,0);
	Macro_Clear_Bit(TIM2->SR, 0);
	Macro_Set_Bit(TIM2->CR1, 0);
	while (Macro_Check_Bit_Clear(TIM2->SR, 0));

	Macro_Clear_Bit(TIM2->CR1, 0);
}

#endif

void TIM4_Delay_Us(int time_us)
{
    // TIM4 클록 활성화
    Macro_Set_Bit(RCC->APB1ENR, 2);

    // Down-counter, Repeat mode 설정
    TIM4->CR1 = (1<<4)|(1<<3);

    // 프리스케일러 설정
    TIM4->PSC = (unsigned int)(TIMXCLK/(double)1000000 + 0.5)-1;

    // 설정한 us에 맞는 ARR 값 설정 (1ms 펄스 수 / 1000 = 1us 펄스 수)
    TIM4->ARR = time_us - 1;

    // 레지스터 업데이트 및 상태 플래그 클리어
    Macro_Set_Bit(TIM4->EGR, 0);
    Macro_Clear_Bit(TIM4->SR, 0);

    // TIM4 인터럽트 활성화
    Macro_Set_Bit(TIM4->DIER, 0);
    NVIC_EnableIRQ(30);

    // TIM4 카운터 시작
    Macro_Set_Bit(TIM4->CR1, 0);
}

void TIM4_Repeat(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 2);

	TIM4->CR1 = (1<<4)|(0<<3);
	TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
	TIM4->ARR = TIME4_PLS_OF_1ms * time - 1;

	Macro_Set_Bit(TIM4->EGR,0);
	Macro_Clear_Bit(TIM4->SR, 0);
	Macro_Set_Bit(TIM4->CR1, 0);
}

int TIM4_Check_Timeout(void)
{
	if(Macro_Check_Bit_Set(TIM4->SR, 0))
	{
		Macro_Clear_Bit(TIM4->SR, 0);
		return 1;
	}
	else
	{
		return 0;
	}
}

void TIM4_Stop(void)
{
	NVIC_DisableIRQ(30);
	Macro_Clear_Bit(TIM4->CR1, 0);
	Macro_Clear_Bit(TIM4->DIER, 0);
}

void TIM4_Change_Value(int time)
{
	TIM4->ARR = TIME4_PLS_OF_1ms * time;
}

void TIM4_Interrupt_Enable(int en, int oneshot, int time)
{
	if(en)
	{
		// TIM4 Clock On
		Macro_Set_Bit(RCC->APB1ENR, 2);

		TIM4->CR1 = (1<<4)|(oneshot<<3);
		TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5)-1;
		TIM4->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM4->EGR,0);

		// TIM4 Pending Clear
		// NVIC Pending Clear
		Macro_Clear_Bit(TIM4->SR, 0);
		NVIC_ClearPendingIRQ(30);

		// TIM4 Interrupt Enable
		// NVIC Interrupt Enable
		Macro_Set_Bit(TIM4->DIER, 0);
		NVIC_EnableIRQ(30);

		// TIM4 Start
		Macro_Set_Bit(TIM4->CR1, 0);
	}
	else
	{
		NVIC_DisableIRQ(30);
		Macro_Clear_Bit(TIM4->CR1, 0);
		Macro_Clear_Bit(TIM4->DIER, 0);
	}
}

#define TIM3_FREQ					(800000)			// Hz
#define TIM3_TICK					(1000000/TIM3_FREQ)	// usec
#define TIME3_PLS_OF_1ms			(1000/TIM3_TICK)

void TIM3_Out_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 1);
	Macro_Set_Bit(RCC->APB1ENR, 1);

	Macro_Write_Block(GPIOB->MODER, 0x3, 0x2, 0);  	// PB0 => ALT
	Macro_Write_Block(GPIOB->AFR[0], 0xf, 0x2, 0); 	// PB0 => AF02

	Macro_Write_Block(TIM3->CCMR2, 0xff, 0x68, 0);
	TIM3->CCER = (0<<9)|(1<<8);
}

void TIM3_Out_PWM_Generation(unsigned short freq, int duty)
{
	// Timer 주파수가 TIM3_FREQ가 되도록 PSC 설정
	TIM3->PSC = (unsigned int)(TIMXCLK/TIM3_FREQ + 0.5)-1;

	// 요청한 주파수가 되도록 ARR 설정
	TIM3->ARR = (int)((double)TIM3_FREQ / freq + 0.5) - 1;

	// Duty Rate를 CCR3 설정
	TIM3->CCR3 = TIM3->ARR * (duty / 10.) / 100;

	// Manual Update(UG 발생)
	Macro_Set_Bit(TIM3->EGR, 0);

	// Down Counter, Repeat Mode, Timer Start
	TIM3->CR1 = (1<<4)|(0<<3);
	Macro_Set_Bit(TIM3->CR1, 0);
}

void TIM3_Out_Freq_Generation(unsigned short freq)
{
	// Timer 주파수가 TIM3_FREQ가 되도록 PSC 설정
	TIM3->PSC = (unsigned int)(TIMXCLK/TIM3_FREQ + 0.5)-1;

	// 요청한 주파수가 되도록 ARR 설정
	TIM3->ARR = (int)((double)TIM3_FREQ / freq + 0.5) - 1;

	// Duty Rate 50%가 되도록 CCR3 설정
	TIM3->CCR3 = TIM3->ARR / 2;

	// Manual Update(UG 발생)
	Macro_Set_Bit(TIM3->EGR, 0);

	// Down Counter, Repeat Mode, Timer Start
	TIM3->CR1 = (1<<4)|(0<<3);
	Macro_Set_Bit(TIM3->CR1, 0);
}

void TIM3_Out_Stop(void)
{
	Macro_Clear_Bit(TIM3->CR1, 0);
}

#define TIM5_FREQ					(8000000)			// Hz
#define TIM5_TICK					(1000000/TIM5_FREQ)	// usec
#define TIME5_PLS_OF_1ms			(1000/TIM5_TICK)

void TIM5_Out_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 0U);
	Macro_Set_Bit(RCC->APB1ENR, 3U);

	Macro_Write_Block(TIM5->CCMR1, 0xffff, 0x6868, 0U);

	TIM5->CCER = (1 << 4) | (1 << 0);
}

void TIM5_Out_PWM_Generation(int duty)
{
	// 입력 범위 제한 (0 ~ 200)
    if (duty < 0) duty = 0;
    if (duty > 200) duty = 200;

	// Timer 주파수가 TIM5_FREQ가 되도록 PSC 설정
	TIM5->PSC = (unsigned int)(TIMXCLK/TIM5_FREQ + 0.5) - 1;

	// 요청한 주파수가 되도록 ARR 설정
	TIM5->ARR = (int)((double)TIM5_FREQ / 10000 + 0.5) - 1;

	if (duty == 0)
    {
        // 속도 0 입력 시 완전 정지
        TIM5->CCR1 = 0;
        TIM5->CCR2 = 0;
    }
    else
    {
        // duty(1 ~ 200)를 듀티비 50.0% ~ 100.0%로 매핑
        // ratio: 500(50.0%) ~ 1000(100.0%)
        int duty_permille = 500 + ((duty * 500) / 200);

        TIM5->CCR1 = (TIM5->ARR * duty_permille) / 1000;
        TIM5->CCR2 = (TIM5->ARR * duty_permille) / 1000;
    }

	// Manual Update(UG 발생)
	Macro_Set_Bit(TIM5->EGR, 0);

	// Down Counter, Repeat Mode, Timer Start
	TIM5->CR1 = (1<<4)|(0<<3);
	Macro_Set_Bit(TIM5->CR1, 0);
}

void TIM5_Out_Stop(void)
{
	Macro_Clear_Bit(TIM5->CR1, 0);
}