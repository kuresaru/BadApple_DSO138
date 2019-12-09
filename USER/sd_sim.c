#include "sd_sim.h"

#define WAIT_COUNT 200
#define WAIT_COUNT_HIGH 30000

struct SD_Info {
	enum SD_Type {
		UNINITIALIZED=0, SD_1, SD_2, SD_HC
	} SD_Type;
} SD_Info;

u8 SD_RW(u8 dat) {
	u8 tmp = 0;
	for (u8 i = 0; i < 8; i++) {
		tmp <<= 1;
		tmp |= (SD_DIN ? 1 : 0);
		if ((dat >> (7 - i)) & 0x1)
			SD_OUT_SET;
		else
			SD_OUT_CLR;
		SD_SCK_SET;
		if (!SD_Info.SD_Type)
			SD_LOWSPEED_DELAY;
		SD_SCK_CLR;
		if (!SD_Info.SD_Type)
			SD_LOWSPEED_DELAY;
	}
	return tmp;
}

u8 SD_SendNOP() {
	return SD_RW(0xFF);
}

u8 SD_SendData(u8 cmd, u32 arg, u8 crc) {
	u8 tmp, wait = WAIT_COUNT;
	SD_SendNOP();
	SD_RW(cmd);
	SD_RW((arg >> 24) & 0xFF);
	SD_RW((arg >> 16) & 0xFF);
	SD_RW((arg >> 8) & 0xFF);
	SD_RW(arg & 0xFF);
	SD_RW(crc);
	do {
		tmp = SD_SendNOP();
	} while (tmp == 0xFF && wait--);
	return tmp;
}

//初始化SD卡 成功返回0 只支持SD2.0
u8 SD_InitCard() {
	u8 tmp, wait;
	SD_ResetCS;
	Delay_ms(1);
	SD_SetCS;
	Delay_ms(1);
	for (tmp = 0; tmp < 16; tmp++) { //初始化时钟
		SD_SendNOP();
	}
	Delay_ms(1);
	SD_ResetCS;
	tmp = SD_SendData(0x40, 0x00000000, 0x95); //CMD0
	if (tmp != 0x01) {
		SD_SetCS;
		return 1;
	}
	tmp = SD_SendData(0x48, 0x000001AA, 0x87); //CMD8
	if (tmp == 0x01) {
		//SD2.0
		SD_Info.SD_Type = SD_2;
		SD_SendNOP(); //接收完CMD8的4个字节
		SD_SendNOP();
		SD_SendNOP();
		SD_SendNOP();
		wait = WAIT_COUNT;
		do {
			Delay_ms(1);
			SD_SendData(0x77, 0x00000000, 0xFF); //CMD55
			tmp = SD_SendData(0x69, 0x40000000, 0xFF); //ACMD41
		} while (tmp != 0x00 && wait--);
		if (tmp == 0x00) {
			SD_SendData(0x7A, 0x00000000, 0xFF); //CMD58
			tmp = SD_SendNOP();
			SD_SendNOP();
			SD_SendNOP();
			SD_SendNOP();
			if (tmp & 0x40) {
				SD_Info.SD_Type = SD_HC;
			}
		} else {
			SD_SetCS;
			return 1;
		}
	} else if (tmp == 0x05) {
		//SD1.0
		SD_Info.SD_Type = SD_1;
	} else {
		SD_SetCS;
		return 1;
	}
	SD_SetCS;
	return 0;
}

u8 SD_StartReadBlock(u32 addr) {
	u8 tmp;
	u16 wait = WAIT_COUNT_HIGH;
	if (SD_Info.SD_Type != SD_HC) { //非HC卡 扇区号转字节号
		addr <<= 9;
	}
	do {
		tmp = SD_SendData(0x51, addr, 0xFF);
	} while (tmp && wait--);
	if (tmp == 0) {
		wait = WAIT_COUNT_HIGH;
		do {
			tmp = SD_SendNOP();
		} while (tmp != 0xFE && wait--);
		if (wait) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}
