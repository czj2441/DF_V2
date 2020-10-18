#include "WS2812B.h"
/* Buffer that holds one complete DMA transmission
 *
 * Ensure that this buffer is big enough to hold
 * all data bytes that need to be sent
 *
 * The buffer size can be calculated as follows:
 * number of LEDs * 24 bytes + 42 bytes
 *
 * This leaves us with a maximum string length of
 * (2^16 bytes per DMA stream - 42 bytes)/24 bytes per LED = 2728 LEDs
 */
#define TIM2_CCR2_Address 0x40000038 //TIM2_CH2 PB3 DMA1_CH7 LED5 重映射引脚
#define TIM3_CCR1_Address 0x40000434 //TIM3_CH1 PB4 DMA1_CH6 LED4 重映射引脚
#define TIM4_CCR1_Address 0x40000834 //TIM4_CH1 PB6 DMA1_CH1 LED2
#define TIM4_CCR2_Address 0x40000838 //TIM4_CH2 PB7 DMA1_CH4 LED3
#define TIM4_CCR3_Address 0x4000083C //TIM4_CH3 PB8 DMA1_CH5 LED1

#define TIMING_ONE  50
#define TIMING_ZERO 25

uint16_t LED1_BYTE_Buffer[4843];//当前值最大使用200串LED 需要更多时要增大该值 否则指针越界 程序进入HARDFAULT
uint16_t LED2_BYTE_Buffer[4843];
uint16_t LED3_BYTE_Buffer[4843];
uint16_t LED4_BYTE_Buffer[4843];
uint16_t LED5_BYTE_Buffer[4843];
//---------------------------------------------------------------//

void ws2812Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    /*使能时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /*初始化GPIO*/
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//失能JTAG 释放PB3
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);//TIM2_CH2通道重映射到PB3引脚
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);//TIM3_CH1通道重映射到PB4引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /*初始化定时器2 3 4时基*/
    TIM_TimeBaseStructure.TIM_Period = 90 - 1; // 800kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    /*初始化定时器输出比较*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    /*初始化TIM2_CH2 DMA1通道7 LED5*/
    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM2->CCR2);	// physical address of Timer 3 CCR1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED5_BYTE_Buffer;		// this is the buffer memory
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// data shifted from memory to peripheral
    DMA_InitStructure.DMA_BufferSize = 42;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    /*初始化TIM3_CH1 DMA1通道6 LED4*/
    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM3->CCR1);	// physical address of Timer 3 CCR1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED4_BYTE_Buffer;		// this is the buffer memory
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    /*初始化TIM4_CH1 DMA1通道1 LED2*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM4->CCR1);	// physical address of Timer 3 CCR1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED2_BYTE_Buffer;		// this is the buffer memory
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /*初始化TIM4_CH2 DMA1通道4 LED3*/
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM4->CCR2);	// physical address of Timer 3 CCR1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED3_BYTE_Buffer;		// this is the buffer memory
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    /*初始化TIM4_CH3 DMA1通道5 LED1*/
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&TIM4->CCR3);	// physical address of Timer 3 CCR1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED1_BYTE_Buffer;		// this is the buffer memory
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    /*初始化TIM DMA事件*/
    TIM_DMACmd(TIM2, TIM_DMA_CC2, ENABLE);
    TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
    TIM_DMACmd(TIM4, TIM_DMA_CC1, ENABLE);
    TIM_DMACmd(TIM4, TIM_DMA_CC2, ENABLE);
    TIM_DMACmd(TIM4, TIM_DMA_CC3, ENABLE);
//	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
//	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
//  TIM_DMACmd(TIM4, TIM_DMA_Update, ENABLE);
}

/* This function sends data bytes out to a string of WS2812s
 * The first argument is a pointer to the first RGB triplet to be sent
 * The seconds argument is the number of LEDs in the chain
 *
 * This will result in the RGB triplet passed by argument 1 being sent to
 * the LED that is the furthest away from the controller (the point where
 * data is injected into the chain)
 */

void ws2812_led1_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;				// reset buffer memory index

    while (len)
    {
        for(i = 0; i < 8; i++) // GREEN data
        {
            LED1_BYTE_Buffer[memaddr] = ((color[0][1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // RED
        {
            LED1_BYTE_Buffer[memaddr] = ((color[0][0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // BLUE
        {
            LED1_BYTE_Buffer[memaddr] = ((color[0][2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//===================================================================//
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
    LED1_BYTE_Buffer[memaddr] = ((color[0][2] << 8) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
//===================================================================//
    memaddr++;
    while(memaddr < buffersize)
    {
        LED1_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    DMA_SetCurrDataCounter(DMA1_Channel5, buffersize); 	// load number of bytes to be transferred
    DMA_Cmd(DMA1_Channel5, ENABLE); 			// enable DMA channel
    TIM_Cmd(TIM4, ENABLE); 						// enable Timer
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC5)) ; 	// wait until transfer complete
    DMA_Cmd(DMA1_Channel5, DISABLE); 			// disable DMA channel
    DMA_ClearFlag(DMA1_FLAG_TC5); 				// clear DMA1 Channel transfer complete flag
    TIM_Cmd(TIM4, DISABLE); 	// disable Timer
}

void ws2812_led2_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;				// reset buffer memory index

    while (len)
    {
        for(i = 0; i < 8; i++) // GREEN data
        {
            LED2_BYTE_Buffer[memaddr] = ((color[0][1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // RED
        {
            LED2_BYTE_Buffer[memaddr] = ((color[0][0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // BLUE
        {
            LED2_BYTE_Buffer[memaddr] = ((color[0][2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//===================================================================//
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
    LED2_BYTE_Buffer[memaddr] = ((color[0][2] << 8) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
//===================================================================//
    memaddr++;
    while(memaddr < buffersize)
    {
        LED2_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    DMA_SetCurrDataCounter(DMA1_Channel1, buffersize); 	// load number of bytes to be transferred
    DMA_Cmd(DMA1_Channel1, ENABLE); 			// enable DMA channel
    TIM_Cmd(TIM4, ENABLE); 						// enable Timer
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC1)) ; 	// wait until transfer complete
    DMA_Cmd(DMA1_Channel1, DISABLE); 			// disable DMA channel
    DMA_ClearFlag(DMA1_FLAG_TC1); 				// clear DMA1 Channel transfer complete flag
    TIM_Cmd(TIM4, DISABLE); 	// disable Timer
}

void ws2812_led3_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;				// reset buffer memory index

    while (len)
    {
        for(i = 0; i < 8; i++) // GREEN data
        {
            LED3_BYTE_Buffer[memaddr] = ((color[0][1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // RED
        {
            LED3_BYTE_Buffer[memaddr] = ((color[0][0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // BLUE
        {
            LED3_BYTE_Buffer[memaddr] = ((color[0][2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//===================================================================//
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
    LED3_BYTE_Buffer[memaddr] = ((color[0][2] << 8) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
//===================================================================//
    memaddr++;
    while(memaddr < buffersize)
    {
        LED3_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    DMA_SetCurrDataCounter(DMA1_Channel4, buffersize); 	// load number of bytes to be transferred
    DMA_Cmd(DMA1_Channel4, ENABLE); 			// enable DMA channel
    TIM_Cmd(TIM4, ENABLE); 						// enable Timer
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC4)) ; 	// wait until transfer complete
    TIM_Cmd(TIM4, DISABLE); 	// disable Timer
    DMA_Cmd(DMA1_Channel4, DISABLE); 			// disable DMA channel
    DMA_ClearFlag(DMA1_FLAG_TC4); 				// clear DMA1 Channel transfer complete flag
    TIM_Cmd(TIM4, DISABLE); 	// disable Timer
}

void ws2812_led4_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;				// reset buffer memory index

    while (len)
    {
        for(i = 0; i < 8; i++) // GREEN data
        {
            LED4_BYTE_Buffer[memaddr] = ((color[0][1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // RED
        {
            LED4_BYTE_Buffer[memaddr] = ((color[0][0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // BLUE
        {
            LED4_BYTE_Buffer[memaddr] = ((color[0][2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//===================================================================//
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
    LED4_BYTE_Buffer[memaddr] = ((color[0][2] << 8) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
//===================================================================//
    memaddr++;
    while(memaddr < buffersize)
    {
        LED4_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    DMA_SetCurrDataCounter(DMA1_Channel6, buffersize); 	// load number of bytes to be transferred
    DMA_Cmd(DMA1_Channel6, ENABLE); 			// enable DMA channel
    TIM_Cmd(TIM3, ENABLE); 						// enable Timer
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC6)) ; 	// wait until transfer complete
    DMA_Cmd(DMA1_Channel6, DISABLE); 			// disable DMA channel
    DMA_ClearFlag(DMA1_FLAG_TC6); 				// clear DMA1 Channel transfer complete flag
    TIM_Cmd(TIM3, DISABLE); 	// disable Timer
}

void ws2812_led5_send(uint8_t (*color)[3], uint16_t len)
{
    uint8_t i;
    uint16_t memaddr;
    uint16_t buffersize;
    buffersize = (len * 24) + 43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
    memaddr = 0;				// reset buffer memory index

    while (len)
    {
        for(i = 0; i < 8; i++) // GREEN data
        {
            LED5_BYTE_Buffer[memaddr] = ((color[0][1] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // RED
        {
            LED5_BYTE_Buffer[memaddr] = ((color[0][0] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        for(i = 0; i < 8; i++) // BLUE
        {
            LED5_BYTE_Buffer[memaddr] = ((color[0][2] << i) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
            memaddr++;
        }
        len--;
    }
//===================================================================//
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
    LED5_BYTE_Buffer[memaddr] = ((color[0][2] << 8) & 0x0080) ? TIMING_ONE : TIMING_ZERO;
//===================================================================//
    memaddr++;
    while(memaddr < buffersize)
    {
        LED5_BYTE_Buffer[memaddr] = 0;
        memaddr++;
    }

    DMA_SetCurrDataCounter(DMA1_Channel7, buffersize); 	// load number of bytes to be transferred
    DMA_Cmd(DMA1_Channel7, ENABLE); 			// enable DMA channel
    TIM_Cmd(TIM2, ENABLE); 						// enable Timer
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC7)) ; 	// wait until transfer complete
    DMA_Cmd(DMA1_Channel7, DISABLE); 			// disable DMA channel
    DMA_ClearFlag(DMA1_FLAG_TC7); 				// clear DMA1 Channel transfer complete flag
    TIM_Cmd(TIM2, DISABLE); 	// disable Timer
}

void ws2812Ctrl(u8 nled, uint8_t (*color)[3], uint16_t len){
	switch(nled){
		case 0:ws2812_led1_send(color, len);break;
		case 1:ws2812_led2_send(color, len);break;
		case 2:ws2812_led3_send(color, len);break;
		case 3:ws2812_led4_send(color, len);break;
		case 4:ws2812_led5_send(color, len);break;
	}
}
