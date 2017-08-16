//Author: Wenjun Wu
#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int row, int col, u16 color)
{
	videoBuffer[OFFSET(row, col, 240)] = color;
}

void drawRect(int row, int col, int height, int width, u16 color)
{

	REG_DMA3SAD = (vu32) &color;
	for (int i = 0; i < height; i++) {
		REG_DMA3DAD = (vu32) (videoBuffer + col + (row+i)*240);
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

int collision(Object* z, Object* c) {
	if (((*z).row<(*c).row+20)&&((*z).row+20>(*c).row)&&((*z).col < (*c).col+30)&&((*z).col+10>(*c).col)) {
		return 1;
	}
	else {
		return 0;
	}
}


void drawImage3(int x, int y, int width, int height, const u16* image) {
	for (int i = 0; i < height; i++) {
		REG_DMA3SAD = (vu32) (image + i*width);
		REG_DMA3DAD = (vu32) (videoBuffer + (x + i) * 240 + y);
		REG_DMA3CNT = width | DMA_ON;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}


void drawChar(int x, int y, char ch, u16 color)
{
	for(int row=0; row<8; row++) {
		for(int col=0; col<6; col++) {
			if(fontdata_6x8[OFFSET(row, col, 6) + ch*48]) {
				setPixel(row+x, col+y, color);
			}
		}
	}
}


void drawString(int x, int y, char *str, u16 color) {
	while(*str) {
		drawChar(x, y, *str++, color);
		y = y + 6;
	}
}