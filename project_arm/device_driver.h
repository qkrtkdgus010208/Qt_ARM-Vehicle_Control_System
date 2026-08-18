#include "stm32f4xx.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

// Uart.c

extern void Uart2_Init(int baud);
extern void Uart2_Send_Byte(char data);
extern void Uart2_RX_Interrupt_Enable(int en);

extern void Uart1_Init(int baud);
extern void Uart1_Send_Byte(char data);
extern void Uart1_Send_String(char *pt);
extern void Uart1_Printf(char *fmt,...);
extern char Uart1_Get_Char(void);
extern char Uart1_Get_Pressed(void);

// SysTick.c

extern void SysTick_Run(unsigned int msec);
extern int SysTick_Check_Timeout(void);
extern unsigned int SysTick_Get_Time(void);
extern unsigned int SysTick_Get_Load_Time(void);
extern void SysTick_Stop(void);
extern void SysTick_Delay_Us(unsigned int usec);
extern void SysTick_Delay_Ms(unsigned int msec);

// Led.c

extern void LED_Init(void);
extern void LED_On(void);
extern void LED_Off(void);

// Clock.c

extern void Clock_Init(void);

// Key.c

extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_Wait_Key_Released(void);
extern void Key_Wait_Key_Pressed(void);
extern void Key_ISR_Enable(int en);

// Timer.c

extern void TIM2_Delay(int time);
extern void TIM2_Stopwatch_Start(void);
extern unsigned int TIM2_Stopwatch_Stop(void);
extern void TIM4_Delay_Us(int time_us);
extern void TIM4_Repeat(int time);
extern int TIM4_Check_Timeout(void);
extern void TIM4_Stop(void);
extern void TIM4_Change_Value(int time);
extern void TIM4_Interrupt_Enable(int en, int oneshot, int time);
extern void TIM3_Out_Init(void);
extern void TIM3_Out_PWM_Generation(unsigned short freq, int duty);
extern void TIM3_Out_Freq_Generation(unsigned short freq);
extern void TIM3_Out_Stop(void);
extern void TIM5_Out_Init(void);
extern void TIM5_Out_PWM_Generation(int duty);
extern void TIM5_Out_Stop(void);

// Motor.c

extern void MOTOR_Init(void);
extern void MOTOR_Stop(void);
extern void MOTOR_CW(void);
extern void MOTOR_CCW(void);

// DHT11.c

void dht11_main(void);

// Handler.c

extern void Handler(void);
extern void Key_Handler(void);
extern void UART_Handler(void);
extern void Timer_Handler(void);

// state_manager.c

extern void State_Management(void);
extern void Motor_State_Management(void);

// controller.c

extern void Motor_Controller(void);

// variable

typedef enum {
    STOP = 0,
    CW,
    CCW,
} Motor_State_t;

typedef enum
{
    OK, 
    STARTUP_TIMEOUT, 
    DATA_TIMEOUT, 
    CHECKSUM_ERROR
} state_t;

extern Motor_State_t prev_motor_state;
extern Motor_State_t cur_motor_state;
extern unsigned char prev_motor_speed;
extern unsigned char cur_motor_speed;

extern volatile unsigned char Key_Pressed;
extern volatile unsigned char Key_Released;
extern volatile unsigned char Uart_Data_In;
extern volatile unsigned char Uart_Data;
extern volatile unsigned char TIM4_Expired;
