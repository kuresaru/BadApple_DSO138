#include "stm32f10x.h"
#include "sd_fs.h"
#include "tft.h"

extern u32 FILE_BadApple_Length;
extern u32 FILE_BadApple_StartClust;
extern u8 FS_SectorPerClust;
extern u8 FS_SectorPerClust;
extern u32 FS_FatUseSector;
extern u32 FS_FatSector;

u8 nextFrameFlag;

void Player_Play() {
	u32 nextClust;
	u32 rootSector = FS_FatSector + FS_FatUseSector + FS_FatUseSector;
	u8 clustSector;
	u16 i, x = 0, y = 0;
	u32 tmp;
	u8 dat, f;
	
	SD_ResetCS;
	for (nextClust = FILE_BadApple_StartClust; nextClust != 0x0FFFFFFF; nextClust = FS_GetNextClust(nextClust)) { // 所有簇
		for (clustSector = 0; clustSector < FS_SectorPerClust; clustSector++) { // 所有扇区
			SD_StartReadBlock(rootSector + ((nextClust - 2) * FS_SectorPerClust) + clustSector); //开始读扇区
			for (i = 0; i < 512; i++) {
				dat = SD_SendNOP();
				for (f = 0; f < 8; f++) {
					u16 color;
					if ((dat >> (7 - f)) & 0x01) {
						color = clWhite;
					} else {
						color = clBlack;
					}
					TFT_SET((color >> 8));
					TFT_WR_CLR;
					TFT_WR_SET;
					TFT_SET(color);
					TFT_WR_CLR;
					TFT_WR_SET;
				}
				x++;
				if (x == 40) { //一行完了
					x = 0;
					y++; //x归零 y+1
					if (y == 240) { //一帧完了
						y = 0;
						do
							tmp = SysTick->CTRL;
						while ((tmp & 0x01) && (!(tmp & (1 << 16))));
						SysTick->CTRL = 0x00;
						SysTick->LOAD = 9000 * 500;
						SysTick->CTRL = 0x01;
						SysTick->VAL = 0;
						//TFT_SetWindow(0, 320, 0, 240);
						//TFT_WR2C;
					}
				}
			}
			SD_SendNOP();//丢弃两个字节的CRC
			SD_SendNOP();
		}
	}
}
