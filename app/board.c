#include "config.h"

//==========================================================================================
//����Ϊucos���õĶ�ʱ���жϣ��ֱ�ΪT0��T3���κ�һ��������ʵ�֡�
//���Ҫ���и�������Ҫ��ȥos_cpu_a.S�ļ��е���Ӧ���֡�
//T0��TIMER0_COMP_vect ��T3:TIMER3_COMPA_vect
//OSC = 8000000Hz
/*����atmega128��8λ��ʱ��Timer0������Ƚ�ƥ�䵥Ԫ������ʱ�ж�,�������趨Ϊ200Hz(5ms)*/

void timer0_init_ucos(void)
{
   TCNT0  = 0x00;   		/*����Timer0������������ʽΪTCNT0~OCR0		*/
   OCR0   = (1000/OS_TICKS_PER_SEC)*F_CPU_M;   		 /*����һ��ƥ���ж� TOPΪOCR0 				*/
   TCCR0  = _BV(WGM01)|_BV(CS02)|_BV(CS01)|_BV(CS00);   /*����������Ƚ�ƥ��CTCģʽ,������OC0�˿�,ʱ��1024Ԥ��Ƶ*/
   TIFR  |= 0x02;   		/*�������Ƚ�ƥ���жϱ�־λ				*/
   TIMSK |= 0x02;   		/*ʹ������Ƚ�ƥ���ж�						*/
   sei();              		/*ʹ��ȫ���ж�								*/
   TCCR0 |= _BV(FOC0);   	/*��������Ƚ�ƥ��							*/
}

//TIMER3 initialize - prescale:1024
// WGM: 4) CTC, TOP=OCRnA
// desired value: 200Hz
// actual value: 200.321Hz (0.2%)
//OSC = 8000000Hz
/*����atmega128��8λ��ʱ��Timer3������Ƚ�ƥ��A��Ԫ������ʱ�ж�,�������趨Ϊ200Hz(5ms)*/
void timer3_init_ucos(void)
{
	TCCR3B = 0x00; 				//stop
	TCNT3H = 0x00; 				//setup
	TCNT3L = 0x00;
	OCR3AH = 0x00;
	OCR3AL = 0x28;
	TCCR3A = 0x00; 				//OC3C���δ����
	TCCR3B = _BV(WGM32)|_BV(CS32)|_BV(CS30); //start Timer CTC 1024��Ƶ
	ETIFR |= _BV(OCF3A);   		/*�������Ƚ�ƥ���жϱ�־λ��д1��0		*/
    ETIMSK|= _BV(OCIE3A);   	/*ʹ������Ƚ�ƥ���ж�						*/
    sei();              		/*ʹ��ȫ���ж�								*/
}

void port_init(void)
{
	PORTC = 0xFF;				//PC4-5ΪKEY1,KEY2,PC0-3ΪLED1-4��ȫ����������
	DDRC  = 0x0F;				//PC4-5Ϊ���룬PC0-3Ϊ���
	PORTE = 0xC0;				//PE7-6�ֱ�Ϊ�ⲿ�ж�7��6�������������٣���Ƶ
	DDRE  = 0x00;				//�ⲿ�жϲ����½��ش�����ֻʹ�����ⲿ�ж�7
}

void board_init(void)
{
	port_init();
	timer3_init_ucos();
	uart_init();
	i2c_init();
	pcf8563_init();
	adc_init();
	key_value_queue_init();
}