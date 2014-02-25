#include "config.h"

void timers_init(void)
{
	timer0_init();						//��ʱ��0��ʼ��
	timer1_init();						//��ʱ��1��ʼ��
	timer2_init();						//��ʱ��2��ʼ��

	TIMER012_TOIE_ENABLE();				//��ʱ��012����ж�ʹ��
}

// TIMER0 initialize - prescale:1024 �����ȡ���ݵ�ʱ��������
// WGM: Normal
// desired value: 20mSec
// actual value: 19.968mSec (0.2%)
void timer0_init(void)
{
	TCCR0 = 0x00; 						//stop
	ASSR  = 0x00; 						//set async mode
	TCNT0 = TIMER0_INIT_VALUE; 			//set count 0x64
	OCR0  = 0x00;

	//TCCR0 = 0x07; 					//start timer
}

// TIMER1 initialize - prescale:Rising edge ���ⲿʱ�ӵ������ؽ��м��� ����ת��
// WGM: 0) Normal, TOP=0xFFFF
// desired value: Hz
// actual value: Out of range
void timer1_init(void)
{
	TCCR1B = 0x00;						//stop
	TCNT1  = 0x00 						/*INVALID SETTING*/; //setup
	OCR1AH = 0x00 						/*INVALID SETTING*/;
	OCR1AL = 0x00 						/*INVALID SETTING*/;
	OCR1BH = 0x00 						/*INVALID SETTING*/;
	OCR1BL = 0x00 						/*INVALID SETTING*/;
	OCR1CH = 0x00 						/*INVALID SETTING*/;
	OCR1CL = 0x00 						/*INVALID SETTING*/;
	ICR1H  = 0x00 						/*INVALID SETTING*/;
	ICR1L  = 0x00 						/*INVALID SETTING*/;
	TCCR1A = 0x00;
	TCCR1C = 0x00;

	/*TCCR1B = 0x07; //start Timer*/
}

void timers_cnt_reset(void)
{
	TIMER0_CNT_RESET();							// Timer0�����ڲ�ʱ��,ÿ��������ʱ��Ϊ20ms
	TIMER1_CNT_RESET();							// ������ֵ����
	TIMER2_CNT_RESET();							// ������ֵ����
}

void timers_stop(void)
{
	TIMER0_STOP();								// ֹͣ������0  ������������ݶ�ȡ�������
	TIMER1_STOP();								// ֹͣ������1	����ת�ز���
	TIMER2_STOP();								// ֹͣ������2  �������ٲ���
}	

void timers_start(void)
{
	TIMER2_START();								// ����������
	TIMER1_START();								// ����������
	TIMER0_START();								// ����������
}

void timers_ovf_times_reset(void)
{
	timer0_ovf_times = 0;						// Timer0�����������
	timer1_ovf_times = 0;						// Timer1�����������
	timer2_ovf_times = 0;						// Timer2�����������
}
// TIMER2 initialize - prescale:Rising edge  ���ⲿʱ�ӵ������ؽ��м��� ��������
// WGM: Normal
// desired value: 1Hz
// actual value: Out of range
void timer2_init(void)
{
	TCCR2 = 0x00; 						//stop
	TCNT2 = 0x00; 						//setup
	OCR2  = 0x00;

	/*TCCR2 = 0x07; 					//�ⲿʱ��Դ*/
}

ISR(TIMER0_OVF_vect)
{
	TIMER0_CNT_RESET();					// TIMER0��ʼֵ���¼���
	timer0_ovf_times ++;				// Timer0���������һ
	adc_convert_start();				// ����ADת��, ����һ�������ѹ

	if(timer0_ovf_times >= timer0_max_ovf_timers)
	{
										// ��ʱʱ�䵽, ֹͣ��ʱ, ������Ӧ����
		measure_stop();
		time_is_out_flag = TRUE;
	}
}

ISR(TIMER1_OVF_vect)
{
	timer1_ovf_times++;					// Timer1���������һ
}

ISR(TIMER2_OVF_vect)
{
	timer2_ovf_times++;					// Timer2���������һ
}

