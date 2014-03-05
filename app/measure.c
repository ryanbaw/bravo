#include "config.h"

//---------------ȫ�ֱ���������--------------------
uint8 timer0_ovf_times = 0x00;					// Timer0�������
uint8 timer1_ovf_times = 0x00;					// Timer1�������
uint8 timer2_ovf_times = 0x00;					// Timer2�������
uint8 timer0_max_ovf_timers = TIME0_MAX_OVF_TIMES;		// Timer0����������,����ȷ����ʱ����
bool time_is_out_flag = FALSE;					// ��ʱʱ�䵽��־
bool continue_measure_flag = FALSE;				// ����������־

uint32 timer1_count = 0;						// T1����ֵ
uint32 timer2_count = 0;						// T2����ֵ

MeasurePara measure_para;						// ���������ṹ��
MeasurePara measure_para_eeprom 	__attribute__((section(".eeprom")));
MeasureFrequence measure_frequence;

//----------------����ʵ��------------------------
static void measure_para_init(void)					// ����������ʼ��
{
	measure_para.current_ratio 			= 1.0f;
	measure_para.current_offset 		= 0.0f;
	measure_para.voltage_ratio 			= 1.0f;
	measure_para.voltage_offset 		= 0.0f;
	measure_para.flow_speed_ratio 		= 1.0f;
	measure_para.flow_speed_offset 		= 0.0f;
	measure_para.torque_ratio 			= 1.0f;
	measure_para.torque_offset 			= 0.0f;
	measure_para.rotate_speed_ratio 	= 1.0f;
	measure_para.rotate_speed_offset	= 0.0f;

	measure_init();
}

void measure_para_write_to_eeprom(void)			//��Ĭ�ϲ����������浽EEPROM��
{
	float value[MEASURE_PARA_SIZE] = {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
	eeprom_write_block(value, &measure_para_eeprom, MEASURE_PARA_SIZE*sizeof(float));
}

void measure_para_read_from_eeprom(void)		//������������EEPROM�ж�ȡ��measure_para��
{
	eeprom_read_block(&measure_para, &measure_para_eeprom, MEASURE_PARA_SIZE*sizeof(float));
}

void measure_init(void)
{
	memset(&measure_frequence, 0x00, sizeof(MeasureFrequence));
}

void measure_start(void)						// ��������
{
	timers_stop();
	timers_ovf_times_reset();				
	timers_cnt_reset();							// Timer0�����ڲ�ʱ��,ÿ��������ʱ��Ϊ20ms
	measure_init();
	TIMER012_TOIE_ENABLE();						// ʹ��Timer0 Timer1 Timer2 ������ж�
	ADC_ON();									// ����AD
	EXTINT_ENABLE();							// �����ⲿ�ж�
	adc_queue.count = 0x00;						// ��������
	adc_convert_start();						// ����ADת��, ����һ�������ѹ
	timers_start();
}

void measure_stop(void)
{
	timers_stop();
	TIMER012_TOIE_DISABLE();					// ��ֹTimer0 Timer1 Timer2 ������ж�
	ADC_OFF();									// �ر�AD
	EXTINT_DISABLE();							// �ر��ⲿ�ж�
}

void measure_time_count_read(void)				// ���ٲ���
{
	measure_frequence.num++;					// ������1

	if(1 == measure_frequence.num)				// ����ǵ�һ��
	{
		measure_frequence.start_time = TCNT0;	// ��¼�¶�ʱ������ֵ����ʼʱ��
		measure_frequence.timer0_start_ovf_times = timer0_ovf_times;//��¼�������
	}
	else
	{
		measure_frequence.end_time = TCNT0;		// ������ǵ�һ�Σ����ٴμ�¼��T0��ǰ����ֵ
		measure_frequence.timer0_end_ovf_times = timer0_ovf_times;	//��¼�������������
	}
}

float measure_data_get(void)
{
	uint16 nFlowTimeCount;
	if(measure_frequence.num <= 1)
		return 0.0f;

	nFlowTimeCount  = measure_frequence.end_time  +((uint16)measure_frequence.timer0_end_ovf_times*(uint16)(256-TIMER0_INIT_VALUE));	// ��ȡ���һ�������ʱ��
	nFlowTimeCount -= measure_frequence.start_time+((uint16)measure_frequence.timer0_start_ovf_times*(uint16)(256-TIMER0_INIT_VALUE));// ��ȥ��һ�������ʱ��

	if(!nFlowTimeCount)							// ���Ϊ��, ����������
		return 0.0f;

	return (7800.0f * measure_para.flow_speed_ratio * (measure_frequence.num-1) / nFlowTimeCount
		+ measure_para.flow_speed_offset);
}

void measure_data_read(void)
{
	timer1_count = /*timer1_ovf_times * 0xFFFF +*/ TCNT1;
	timer2_count = ((uint16)timer2_ovf_times << 8) + TCNT2;

	// ��ȡ����
	data_buf.data.fTorque = 
		timer1_count * 40.0f / timer0_max_ovf_timers * measure_para.torque_ratio + measure_para.torque_offset;
	data_buf.data.fRotateSpeed = 
		timer2_count * 40.0f / timer0_max_ovf_timers * measure_para.rotate_speed_ratio + measure_para.rotate_speed_offset;

	adc_queue_get_data(&data_buf.data.fCurrent, &data_buf.data.fVoltage, &data_buf.data.fPower );
	data_buf.data.fFlowSpeed = measure_data_get();

	// �����ݻ��������������ݷŵ����ݷ��Ͷ�����
	data_queue_add_data(&data_buf.data);		// ��ӵ����ݷ��Ͷ�����

	if(continue_measure_flag)					// �������Ҫ��������		
		measure_start();						// ���¿�ʼ����
}

void timer0_max_ovf_timers_set(uint8 num)		// ���ö�ʱ��0��������ֵ��>1��
{
	timer0_max_ovf_timers = max(num, 1);
}

void measure_para_set(uint8 num, float value)	// ������������
{
	measure_para.value[num] = value;
}

float measure_para_read(uint8 num)				// ��ȡ��������
{
	return measure_para.value[num];
}
