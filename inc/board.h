#ifndef __BOARD_H__
#define __BOARD_H__

#define LED1				0
#define LED2				1
#define LED3				2
#define LED4				3

//---------�˿ڿ��ƣ�H��������ߵ�ƽ��L��������͵�ƽ��R���������ȡ��------------
#define GPIO_OUT_H(io, x);	{DDR##io |= (1 << (x & 0x07)); PORT##io |=  (1 << (x & 0x07));}
#define GPIO_OUT_L(io, x);	{DDR##io |= (1 << (x & 0x07)); PORT##io &= ~(1 << (x & 0x07));}
#define GPIO_OUT_R(io, x);	{DDR##io |= (1 << (x & 0x07)); PORT##io ^=  (1 << (x & 0x07));}

void timer0_init_ucos(void);
void timer3_init_ucos(void);
void port_init(void);
void board_init(void);

#endif