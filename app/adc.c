#include "config.h"
//ע����ADCת��ʱӦ���ȶ�ADCӲ�����г�ʼ����Ȼ��ѡ��ͨ��������
//��� ������ADCת�����������У�ADCֻ�ܽ��е���ת������������ת����
//���⣬ADC���в���ʱ��һ��Ҫ�ȼ������ֵ ���ټ����ѹֵ����ס��Ҫ����
//��������Ԫ�ص�ָ�롣
//-----------------------
// �������
AdcStatusFlag adc_status;							// ADC״̬�ṹ�����

unsigned int  adc_result;							// ������ת�����
unsigned char adc_mean_num = ADC_MAX_SIZE;			// �ڻ�����ƽ�ؾ����㷨��, ʱ�䴰�Ĵ�С,(��������Ķ��ٸ�ֵ��ƽ��)
AdcQueue 	  adc_queue;							// �����ڻ��ζ���, ���������ת�����

//-----------------------
void adc_init(void)
{
	ADC_OFF();										// �ر�ADC
	ADMUX = _BV(REFS0)|_BV(MUX1)|_BV(MUX0); 		// �ⲿ�ο���ԴVACC��ѡ��ADC3���գ�
	ACSR  = _BV(ACD);								// ��ֹACD���ر����Դ
	ADC_ON();										// ��ADC,ʹ��ADC�жϣ����������2��Ƶ
	adc_queue_init();
}

static void adc_switch_channel(bool channel)		// �л�ADת��ͨ��
{
	switch(channel)
	{
		case CURRENT:
			ADMUX = _BV(REFS0) | ADC_CURRENT; 		// ADC1 ����
			break;
			
		case VOLTAGE:
			ADMUX = _BV(REFS0) | ADC_VOLTAGE; 		// ADC2 ��ѹ
			break;
			
		default:
			break;
	}
}
// AD�жϷ������ȱ���ǵ�ѹ��������ת�����Σ���������һ�βɼ�ֵ

ISR(ADC_vect)										
{
	if(adc_status.who_is_convert == CURRENT)		// �����������ת��
	{
		if(adc_status.first_time == TRUE)			// ���ҵ�һ�ο�ʼת��
		{
			adc_status.first_time = FALSE;			// �õ�һ��ת�����ΪF
			START_ADC();							// ����ADת��
		}
		else
		{
			adc_result = ADC;						// �������ת��ֵ
			adc_queue_add_current(adc_result);		// ������ֵ���뵽ADC������

			adc_status.first_time = TRUE;			// �����õ�һ��ת������ΪT
			adc_status.who_is_convert = VOLTAGE;	// �����ת������ɣ�������ΪF

			adc_switch_channel(VOLTAGE);			// �л�ͨ������ѹת��
			START_ADC();							// ����ADת��
		}
	}
	else if(adc_status.who_is_convert == VOLTAGE)	// �����ǰ��ѹ����ת��
	{
		if(adc_status.first_time == TRUE)			// �ǵ�һ��ת��
		{	
			adc_status.first_time = FALSE;			// �ñ��ΪF
			START_ADC();							// ����ADת����ת����ѹ��
		}
		else										// ������ǵ�һ��
		{
			adc_result = ADC;						// ���ȡ��ѹֵ
			adc_queue_add_voltage(adc_result);		// ����ѹֵ����ADC������
		}
	}
}

//---------------ADC���к�������------------------
void adc_queue_init(void)
{
	uint8 *p = (uint8 *)&adc_queue;
	memset(p, 0x00, sizeof(AdcQueue));		// ���ADC���л�����
}

void adc_convert_start(void)
{
	memset(&adc_status, 0, sizeof(AdcStatusFlag));
	adc_status.who_is_convert 	= CURRENT;			// ��λ����ת�����ȵ������ѹ˳��
	adc_status.first_time 		= TRUE;				// ��һ��ת��
	adc_switch_channel(CURRENT);					// �л�������ת��

	START_ADC();									// ����һ��ת����û�в�������ת����
}

void adc_queue_add_current(short int data)
{
	adc_queue.data[adc_queue.tail].current = data;	// �ڶ���β����ӵ�������
}

void adc_queue_add_voltage(short int data)			
{
	adc_queue.data[adc_queue.tail].voltage = data;	// �ڶ���β����ӵ�ѹ����
	//adc_queue.data[adc_queue.tail].wPower = data * adc_queue.data[adc_queue.tail].current;

	ADCQUEUE_TAIL_ADD();							// ����β���±�

	adc_queue.count++; 								// ���ݸ�����1
	adc_queue.count = min(adc_queue.count, ADC_MAX_SIZE);  //ADC_MAX_SIZE=64
}

void adc_mean_num_set(unsigned char num)			//���õķ�Χ��1��64
{
	adc_mean_num = min(num, ADC_MAX_SIZE);
	adc_mean_num = max(num, 1);
}

void adc_queue_get_data(float* pcurrent, float* pvoltage, float* ppower)
{
	unsigned char i;
	unsigned char n;
	unsigned char tail;
	short int res_current 	= 0;
	short int res_voltage 	= 0;
	short int temp_current 	= 0;
	short int temp_voltage 	= 0;
	float 	  resPower 		= 0.0f;

	n = min(adc_mean_num, adc_queue.count);			// ȡ��ǰ���������ݸ���������ֵ����Сֵ
	if(0 == n)
		return;

	tail = adc_queue.tail;							// ��ȡ����β���±�
	tail = (tail-1) & ADC_MAX_SIZE_MASK;			// ��1֮��,ָ���������һ������
													// ��1����Ϊ�±��Ǵ�0��ʼ��
	for(i = 0; i < n; i++)
	{
		temp_current = adc_queue.data[tail].current;
		temp_voltage = adc_queue.data[tail].voltage;
		res_current += temp_current;
		res_voltage += temp_voltage;

		resPower += (5.0f * temp_current * measure_para.current_ratio / 1024.0f + measure_para.current_offset) * 
			(5.0f * temp_voltage * measure_para.voltage_ratio / 1024.0f + measure_para.voltage_offset);

		tail = (tail-1) & ADC_MAX_SIZE_MASK;
	}

	*pcurrent = 5.0f * res_current * measure_para.current_ratio / n / 1024.0f + measure_para.current_offset;
	*pvoltage = 5.0f * res_voltage * measure_para.voltage_ratio / n / 1024.0f + measure_para.voltage_offset;
	*ppower = resPower / n;
}
/*
ISR(ADC_vect)
{
	adc_result = ADC;
	adc_queue_add_voltage(adc_result);
}
*/
#ifdef ADC_TEST
void adc_test(void)
{
	unsigned char i;
	unsigned char n;
	unsigned char tail;
	short int res_voltage 	= 0;
	short int temp_voltage 	= 0;
	uint16 temp_adc = 0;
	float pvoltage = 0;

	adc_switch_channel(VOLTAGE);
	for(i = 0; i < 50; i++)
	{
		START_ADC();
		temp_adc +=  adc_result;
	}
	
	n = min(adc_mean_num, adc_queue.count);			// ȡ��ǰ���������ݸ���������ֵ����Сֵ
	if(0 == n)
		return;

	tail = adc_queue.tail;							// ��ȡ����β���±�
	tail = (tail-1) & ADC_MAX_SIZE_MASK;			// ��1֮��,ָ���������һ������
													// ��1����Ϊ�±��Ǵ�0��ʼ��
	for(i = 0; i < n; i++)
	{
		temp_voltage = adc_queue.data[tail].voltage;
		res_voltage += temp_voltage;

		tail = (tail-1) & ADC_MAX_SIZE_MASK;
	}
	pvoltage = 5.0f * res_voltage / n / 1024.0f;
	uart_putnstring((uint8 *)&pvoltage, 4);
	OSTimeDly(OS_TICKS_PER_SEC/2);
}
#endif
