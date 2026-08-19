#include "device_driver.h"
#include <stdio.h>
#include <string.h>

#define PA4 4U

void DHT11_Controller(void)
{
    // 변수 선언
    uint8_t bytes[6];
    DHT11_State_t state;
    int us_counter;

    // 변수 초기화
    memset(bytes, 0, sizeof(bytes));
    state = OK;
    us_counter = 0;

    // --------------------------------------------------
    // ---------- start up signal scenario ----------
    // --------------------------------------------------

    // 1. start up signal 전송
    // --- 1.1 reset DHT11 (Output Mode: MODER [1:0] -> 01, ODR Pin 0 -> 1)
    Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, PA4 * 2); // Pin 0: Output Mode (01)
    Macro_Set_Bit(GPIOA->ODR, PA4);                 // Pin 0: High
    TIM2_Delay(100);

    // --- 1.2 low 최소 18ms
    Macro_Clear_Bit(GPIOA->ODR, PA4);               // Pin 0: Low
    TIM2_Delay(20);

    // --- 1.3 high 최소 20 ~ 40us
    Macro_Set_Bit(GPIOA->ODR, PA4);                 // Pin 0: High
    SysTick_Delay_Us(30);

    // 2. start signal 응답 check (Input Mode: MODER [1:0] -> 00)
    Macro_Write_Block(GPIOA->MODER, 0x3, 0x0, PA4 * 2); // Pin 0: Input Mode (00)

    // 2.1 DHT11이 low로 응답하는지 check (최대 100us 대기)
    while (GPIOA->IDR & (1 << PA4))
    {
        SysTick_Delay_Us(1);
        if (++us_counter > 100) { state = STARTUP_TIMEOUT; break; }
    }

    // 2.2 low 구간 유지 확인 (80us)
    if (state == OK)
    {
        us_counter = 0;
        while (!(GPIOA->IDR & (1 << PA4)))
        {
            SysTick_Delay_Us(1);
            if (++us_counter > 100) { state = STARTUP_TIMEOUT; break; }
        }
    }

    // 2.3 high 구간 유지 확인 (80us)
    if (state == OK) // 이후는 start data transmission
    {
        us_counter = 0;
        while (GPIOA->IDR & (1 << PA4))
        {
            SysTick_Delay_Us(1);
            if (++us_counter > 100) { state = STARTUP_TIMEOUT; break; }
        }
    }

    // --------------------------------------------------
    // ---------- Data Read Part ----------
    // --------------------------------------------------

    // 3.1 data 40bits 읽는 작업
    if (state == OK)
    {
        for (int i = 0; i < 5; i++)
        {
            uint8_t one_byte = 0;

            for (int j = 0; j < 8; j++)
            {
                // 1. low 구간 50us check
                us_counter = 0;
                while (!(GPIOA->IDR & (1 << PA4)))
                {
                    SysTick_Delay_Us(1);
                    if (++us_counter > 100) { state = DATA_TIMEOUT; break; }
                }

                // 2. high 구간 길이 check (0: 26 ~ 28us, 1: 70us)
                us_counter = 0;
                while (GPIOA->IDR & (1 << PA4))
                {
                    SysTick_Delay_Us(1);
                    if (++us_counter > 100) { state = DATA_TIMEOUT; break; }
                }

                // 30us 이상이면 1로 판단
                if (us_counter > 30) one_byte |= 1 << (7 - j);
            }

            bytes[i] = one_byte;
        }
    }

    // 3.2 checksum 확인
    uint8_t checksum = 0;
    if (state == OK)
    {
        checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
        if (bytes[4] != checksum) state = CHECKSUM_ERROR;
    }

    // 4. 결과 출력 및 Qt 전송
    switch (state)
    {
        case OK:
            // Qt 수신 포맷: $[온도],[습도]\n
            printf("$%d,%d\n", bytes[2], bytes[0]);
            break;
        case STARTUP_TIMEOUT:
            printf("$ERR,STARTUP_TIMEOUT\n");
            break;
        case DATA_TIMEOUT:
            printf("$ERR,DATA_TIMEOUT\n");
            break;
        case CHECKSUM_ERROR:
            printf("$ERR,CHECKSUM_ERROR\n");
            break;
    }
}