typedef unsigned char u8;
typedef unsigned short u16;
typedef volatile unsigned int vu32;

#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define CYAN COLOR(0,31,31)
#define MAGENTA COLOR(31, 0,31)
#define YELLOW COLOR(31,31,0)
#define WHITE COLOR(31,31,31)
#define BLACK 0
#define SCANLINECOUNTER  (*(volatile unsigned short *)0x4000006)

#define BG2_ENABLE (1 << 10)
#define MODE3 3

#define RGB(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#define WIDTH 240
#define HEIGHT 160

#define OFFSET(r, c, numcols) ((r)*(numcols)+(c))
#define win 1;
#define over 0;

/* Buttons */

#define BUTTON_A      (1<<0)
#define BUTTON_B      (1<<1)
#define BUTTON_SELECT (1<<2)
#define BUTTON_START  (1<<3)
#define BUTTON_RIGHT  (1<<4)
#define BUTTON_LEFT   (1<<5)
#define BUTTON_UP     (1<<6)
#define BUTTON_DOWN   (1<<7)
#define BUTTON_R      (1<<8)
#define BUTTON_L      (1<<9)

#define BUTTONS *(volatile unsigned int *)0x4000130

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)


/* DMA */

typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMAREC *)0x040000B0)

#define REG_DMA0SAD         *(vu32*)0x40000B0 		// source address
#define REG_DMA0DAD         *(vu32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(vu32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(vu32*)0x40000BC 		// source address
#define REG_DMA1DAD         *(vu32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(vu32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(vu32*)0x40000C8 		// source address
#define REG_DMA2DAD         *(vu32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(vu32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(vu32*)0x40000D4 		// source address
#define REG_DMA3DAD         *(vu32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(vu32*)0x40000DC       // control register

/* Defines*/
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

/* Destination address flags */
#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

/* Source address flags */
#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

/* How much to copy flags */
#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

/* When to DMA flags */
#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)

#define DMA_ON (1 << 31)

#define START_ON_FIFO_EMPTY 0x30000000

extern u16* videoBuffer;


typedef struct {
    int row;
    int col;
    int rowOrg;
    int colOrg;
    int dir;
} Object; 

// Prototype
void setPixel(int row, int col, u16 color);
void drawRect(int row, int col, int height, int width, u16 color);
int boundaryCheck(int *var, int bound, int *delta, int size);
void waitForVblank();
void drawImage3(int r, int c, int height, int width, const u16* image);
void drawRect(int r, int c, int height, int width, u16 color);
int collision(Object* z, Object* c);
void drawChar(int row, int col, char ch, u16 color);
void drawString(int row, int col, char *s, u16 color);



extern const unsigned char fontdata_6x8[12288];


