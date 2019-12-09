#include "stm32f10x.h"
#include	"Common.h"

// ===================================
// 配置LCD的IO
#define TFT_RST_SET GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define TFT_RST_CLR GPIO_ResetBits(GPIOB, GPIO_Pin_11)
#define TFT_RD_SET GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define TFT_RD_CLR GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define TFT_WR_SET GPIO_SetBits(GPIOC, GPIO_Pin_15)
#define TFT_WR_CLR GPIO_ResetBits(GPIOC, GPIO_Pin_15)
#define TFT_RS_SET GPIO_SetBits(GPIOC, GPIO_Pin_14)
#define TFT_RS_CLR GPIO_ResetBits(GPIOC, GPIO_Pin_14)
#define TFT_CS_SET GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define TFT_CS_CLR GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define TFT_SET(dat) { GPIOB->ODR = (GPIOB->ODR & 0xFF00) | dat; }

// ===================================
// 显示设置
#define USE_HORIZONTAL

// ===================================
// 其它
#define		ScreenX0 		0
#define		ScreenY0		0
#ifdef USE_HORIZONTAL
	#define		ScreenXsize		320
	#define		ScreenYsize		240
#else
	#define		ScreenXsize		240
	#define		ScreenYsize		320
#endif

#define TFT_WR2C TFT_WriteCom(0x2C)

#define		Rbits			0		// Red bits position
#define		Gbits			5		// Green bits position
#define		Bbits			11		// Blue bits position
#define		RGB(R, G, B)	(((R >> 3) << Rbits) | ((G >> 2) << Gbits) | ((B >> 3) << Bbits))

enum  COLOR {
	clBlack 			=	RGB(0, 0, 0),
	clWhite			= 	RGB(255, 255, 255),
	clRed			= 	RGB(255, 0, 0),
	clGreen			= 	RGB(0, 255, 0),
	clBlue			= 	RGB(0, 0, 255),
	clYellow			=	RGB(255, 255, 0),
	clGainsboro		=	RGB(220, 220, 220),
	clNavy			=	RGB(0, 0, 128),
	clAqua			=	RGB(0, 255, 255),
	clHotpink		=	RGB(255, 105, 180),
	clOrange		=	RGB(255, 165, 0),
	clDeepskyblue	=	RGB(0, 191, 255),
	clDimgray		=	RGB(105, 105, 105),
	cllightsalmon	=	RGB(255, 160, 122),
	cllightcoral		=	RGB(240, 128, 128),
	clpaleturquoise	=	RGB(175, 238, 238),
	clturquoise		=	RGB(64, 224, 208),
	clViolet			=	RGB(238, 130, 238),
	clGray1			=	RGB(90, 90, 90),
	clGray2			=	RGB(220, 220, 220),
	clGray3			=	RGB(240, 240, 240),
	clDarkgray		=	RGB(169, 169, 169),
	clSkyblue		= 	RGB(135, 206, 235),
	clChocolate		= 	RGB(210, 105, 30),
	clMediumseagreen	= 	RGB(60, 179, 113),
	clPeachpuff		=	RGB(255, 218, 185),
	clSeagreen		=	RGB(46, 139, 87),
};

void TFT_Init(void);
void TFT_WriteCom(u8 commport);
void TFT_WriteData(u8 data);
void TFT_Cls(void);
void TFT_SetWindow(u16 x, u16 xsize, u16 y, u16 ysize);
void TFT_Fill(s16 x, s16 y, s16 xsize, s16 ysize, u16 color);
void TFT_WriteChar(u16 x, u16 y, u8 ch, u16 fgcolor, u16 bgcolor);
void TFT_WriteString(u16 x, u16 y, u8 *str, u16 fgcolor, u16 bgcolor);
