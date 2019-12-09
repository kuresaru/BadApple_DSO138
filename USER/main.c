#include "stm32f10x.h"

#include "common.h"
#include "tft.h"
#include "sd_sim.h"
#include "sd_fs.h"
#include "player.h"
#include "logo.h"

void	Clock_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void Port_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //SW7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if (!(GPIOB->IDR & (1 << 12))) {
		while(1);
	}

	GPIO_InitStructure.GPIO_Pin = 0x0CFF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR = 0xFFFF;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIOC->ODR = 0xFFFF;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13;//SD CS MOSI SCK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //SD MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIOA->ODR = 0x19FF;
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //关闭JTAG功能 让PB3 PB4 PA13 PA14为普通IO
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

int main (void) {
	Clock_Init();
	Port_Init();
	TFT_Init();
	TFT_CS_CLR;
	TFT_Cls();
	TFT_WriteString(0, 16*0, (u8 *)"DSO138 OSC BadApple!! > By Kuresaru", clRed, clturquoise);
	TFT_WriteString(0, 16*1, (u8 *)"https://space.bilibili.com/15858903", clHotpink, clAqua);
	TFT_SetWindow(0, 320, 48, 80);
	TFT_WR2C;
	for (u16 i = 0; i < 25600; i++) {
			TFT_SET(LOGO[i] >> 8);
			TFT_WR_CLR;
			TFT_WR_SET;
			TFT_SET(LOGO[i]);
			TFT_WR_CLR;
			TFT_WR_SET;
	}
	TFT_WriteString(0, 16*11, (u8 *)"SD Initializing", clGainsboro, clBlack);
	if (SD_InitCard()) {
		TFT_WriteString(0, 16*12, (u8 *)"SD Initialize Failed", clRed, clBlack);
	} else {
		TFT_WriteString(0, 16*12, (u8 *)"SD Initialize OK", clGreen, clBlack);
		if (FS_Init()) {
			TFT_WriteString(0, 16*13, (u8 *)"FS Initialize OK", clGreen, clBlack);
			if (FS_FindBadAppleBin()) {
				TFT_WriteString(0, 16*14, (u8 *)"BadApple.bin Found", clYellow, clBlack);
				while (GPIOB->IDR & (1 << 12));
				TFT_Cls();
				TFT_SetWindow(0, 320, 0, 240);
				TFT_WR2C;
				Player_Play();
			} else {
				TFT_WriteString(0, 16*14, (u8 *)"BadApple.bin Not Found", clRed, clBlack);
			}
		} else {
			TFT_WriteString(0, 16*3, (u8 *)"FS Initialize Failed", clRed, clBlack);
		}
	}
	TFT_CS_SET;
	while(1);
}
