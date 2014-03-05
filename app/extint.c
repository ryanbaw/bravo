#include "config.h"

void ExtInt_init(void)
{
	EICRA = 0x00; 						//extended ext ints
	EICRB = _BV(ISC61) | _BV(ISC71); 	//�趨�ⲿ�ж�6��7Ϊ�½��ش���
	//EXTINT_ENABLE();					//ʹ���ⲿ�ж�6��7������������
	EXTINT_DISABLE();
}

ISR(INT6_vect)
{

}

ISR(INT7_vect)
{
	measure_time_count_read();
	REVERSE(BITNUM_3);
}
