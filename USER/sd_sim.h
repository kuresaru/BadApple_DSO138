#include "stm32f10x.h"
#include "common.h"

#define SD_SetCS GPIOA->ODR |= (1 << 9)
#define SD_ResetCS GPIOA->ODR &= ~(1 << 9)

#define SD_SCK_SET GPIOA->ODR |= (1 << 13)
#define SD_SCK_CLR GPIOA->ODR &= ~(1 << 13)

#define SD_OUT_SET GPIOA->ODR |= (1 << 10)
#define SD_OUT_CLR GPIOA->ODR &= ~(1 << 10)

#define SD_DIN GPIOA->IDR & (1 << 14)

#define SD_LOWSPEED_DELAY Delay_ms(1)

u8 SD_InitCard(void); //成功返回0
u8 SD_StartReadBlock(u32 addr); //成功返回1
u8 SD_SendNOP(void); //返回读到的数据
