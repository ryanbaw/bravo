#ifndef __MEASURE_H
#define __MEASURE_H

#define MEASURE_PARA_SIZE			10
#define TIME0_MAX_OVF_TIMES			20			// timer0����������������ȷ����ʱ����

typedef struct
{
	union
	{
		struct
		{
			float current_ratio;				// ��������
			float current_offset;				// ����ƫ����
			float voltage_ratio;				// ��ѹ����
			float voltage_offset;				// ��ѹƫ����
			float flow_speed_ratio;				// ��������
			float flow_speed_offset;			// ����ƫ����
			float torque_ratio;					// ת������
			float torque_offset;				// ת��ƫ����
			float rotate_speed_ratio;			// ת������
			float rotate_speed_offset;			// ת��ƫ����
		};
		float value[MEASURE_PARA_SIZE];
	};
}MeasurePara;

typedef struct
{
	uint8 num;								// ��ֵ
	uint8 start_time;						// ����ʱ�����ֵ
	uint8 timer0_start_ovf_times;			// ��ʱ��0��ʼ�������
	uint8 end_time;							// ����ʱ�����ֵ
	uint8 timer0_end_ovf_times;				// ��ʱ��0�����������
}MeasureFrequence;

extern MeasurePara measure_para;
extern MeasurePara measure_para_eeprom  __attribute__((section(".eeprom")));
extern MeasureFrequence measure_frequence;

extern uint8 timer0_ovf_times;				// Timer0�������
extern uint8 timer1_ovf_times;				// Timer1�������
extern uint8 timer2_ovf_times;				// Timer2�������
extern uint8 timer0_max_ovf_timers;			// Timer0����������,����ȷ����ʱ����

extern bool time_is_out_flag;				// ��ʱʱ�䵽��־
extern bool continue_measure_flag;

//void  measure_para_config_init(void);
void  measure_para_write_to_eeprom(void);
void  measure_para_read_from_eeprom(void);
void  measure_init(void);
void  measure_start(void);
void  measure_stop(void);
void  measure_time_count_read(void);
float measure_data_get(void);
void  measure_data_read(void);
void  timer0_max_ovf_timers_set(uint8 num);
void  measure_para_set(uint8 num, float fValue);
float measure_para_read(uint8 num);

#endif
