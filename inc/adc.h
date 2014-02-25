#ifndef __ADC_H_
#define __ADC_H_

//-----------------------
// ADC���ݶ��е���󳤶�
#define ADC_MAX_SIZE			64
#define ADC_MAX_SIZE_MASK		0x3F

#define ADC0 					0
#define ADC1 					1
#define ADC2 					2
#define ADC3 					3
#define ADC4 					4
#define ADC5 					5
#define ADC6 					6
#define ADC7 					7

#define ADC_CURRENT				ADC1
#define ADC_VOLTAGE				ADC2

#define CURRENT					1
#define VOLTAGE					2
//-----------------------

#define	START_ADC();			{ADCSRA |= _BV(ADSC);} // ����ADC����
#define	ADC_ON();				{ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS0);} //2��Ƶ
#define	ADC_OFF();				{ADCSRA = 0x00;}

#define ADCQUEUE_TAIL_ADD();	{adc_queue.tail = (adc_queue.tail + 1) & ADC_MAX_SIZE_MASK;}

// ��ʶADCת����״̬
typedef struct
{
	bool who_is_convert;				// ��ʶ���ڽ��е���ת��(current), ���ǵ�ѹת��(voltage)
	bool first_time;					// ��ʶ�˴�ת������Ƿ����л�ͨ����ĵ�һ��ת�����
}AdcStatusFlag;

typedef struct
{
	int16 current;						//����
	int16 voltage;						//��ѹ
	//long int wPower;
}Pair;

typedef struct
{
	uint8 tail;							//β���
	uint8 count;						//����
	Pair data[ADC_MAX_SIZE];			//��ŵ�����
} AdcQueue;

//-----------------------
extern AdcStatusFlag adc_status;
extern uint8 adc_mean_num;				// �ڻ�����ƽ�ؾ����㷨��, ʱ�䴰�Ĵ�С,(��������Ķ��ٸ�ֵ��ƽ��)
extern AdcQueue adc_queue;				// �����ڻ��ζ���, ���������ת�����

//-----------------------
void adc_init(void);
//void adc_switch_channel(bool channel);
ISR(ADC_vect);

//-----------------------
void adc_queue_init(void);
void adc_convert_start(void);
void adc_queue_add_current(int16 data);
void adc_queue_add_voltage(int16 data);
void adc_queue_get_data(float* pcurrent, 
						float* pvoltage,
						float* ppower);
void adc_mean_num_set(uint8 num);

#ifdef ADC_TEST
void adc_test(void);
#endif

#endif
