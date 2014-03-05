#ifndef __TIMER_H__
#define __TIMER_H__

#define  TIMER0_STOP();				{TCCR0 = 0x00;}
#define  TIMER0_START();			{TCCR0 = 0x07;} 	// 1024��Ƶ

#define  TIMER1_STOP();				{TCCR1B = 0x00;}
#define  TIMER1_START();			{TCCR1B = 0x07;}	// �ⲿT1����Ϊʱ��Դ������������

#define  TIMER2_STOP();				{TCCR2 = 0x00;}
#define  TIMER2_START();			{TCCR2 = 0x07;}		// �ⲿT2����Ϊʱ��Դ������������

#define  TIMER3_STOP();				{TCCR3B = 0x00;}
#define  TIMER3_START();			{TCCR3B = 0x02;}	// 8��Ƶ���ɼ�ʱδ��T3��T3������TICK��1024��Ƶ��

#define TIMER0_IS_OFF()				(TCCR0 == 0x00)
#define TIMER1_IS_OFF()				(TCCR1B == 0x00)
#define TIMER2_IS_OFF()				(TCCR2 == 0x00)
#define TIMER3_IS_OFF()				(TCCR3B == 0x00)

#define TIMER0_IS_ON()				(TCCR0 != 0x00)
#define TIMER1_IS_ON()				(TCCR1B != 0x00)
#define TIMER2_IS_ON()				(TCCR2 != 0x00)
#define TIMER3_IS_ON()				(TCCR3B != 0x00)

// Timer0 �����ڲ�ʱ��,ÿ��������ʱ��Ϊ20ms��1024��Ƶ��
#define TIMER0_INIT_VALUE			0x64
#define TIMER0_CNT_RESET();			{TCNT0 = TIMER0_INIT_VALUE;}
// ������ֵ����
#define TIMER1_CNT_RESET();			{TCNT1 = 0x00;}
// ������ֵ����
#define TIMER2_CNT_RESET();			{TCNT2 = 0x00;}
// ������ֵ����
#define TIMER3_CNT_RESET();			{TCNT3 = 0x00;}

// ʹ��Timer0 Timer1 Timer2 ������ж�
#define TIMER012_TOIE_ENABLE();		{TIMSK = _BV(TOIE0) | _BV(TOIE1) | _BV(TOIE2);}
// ��ֹTimer0 Timer1 Timer2 ������ж�
#define TIMER012_TOIE_DISABLE();	{TIMSK = 0x00;}

void timers_init(void);
void timer0_init(void);
void timer1_init(void);
void timer2_init(void);

void timers_stop(void);
void timers_start(void);
void timers_ovf_times_reset(void);
void timers_cnt_reset(void);

ISR(TIMER0_OVF_vect);
ISR(TIMER1_OVF_vect);
ISR(TIMER2_OVF_vect);

#endif
