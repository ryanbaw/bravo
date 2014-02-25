/************************************************
�ļ���pcf8563.h
��;��pcf8563����ͷ�ļ�
ע�⣺ϵͳʱ��8M
************************************************/
#ifndef __PCF8563_H__
#define __PCF8563_H__

//������ַ��ʽ�� 1 0 1 0   0 0 A0 R/W	
//				 1 0 1 0   0 0  1  0 -- A2	д����
//				 1 0 1 0   0 0  1  1 -- A3  ������ 	
#define WR_ADDR_CMD 			0xA2				//д��ַ����
#define RD_ADDR_CMD 			0xA3				//����ַ����

//---------------����״̬��-------------------
#define SLA_W 					0x18				//SLA+W�ѷ��ͣ������յ�ACK
#define ERR_SLA_W 				0x01				//���մ���

//-------------PCF8563�ڲ��Ĵ�����ַ----------
#define PCF8563_CSR1			0x00				//����״̬�Ĵ���1
#define PCF8563_CSR2			0x01				//����״̬�Ĵ���2
#define PCF8563_CLKOUT			0x0D				//CLKOUTƵ�ʼĴ���
#define PCF8563_TCR				0x0E				//��ʱ�����ƼĴ���
#define PCF8563_RTDR			0x0F				//��ʱ��������ֵ�Ĵ���

#define PCF8563_SEC				0x02				//��Ĵ���
#define PCF8563_MIN				0x03				//�ּĴ���
#define PCF8563_HOU				0x04				//ʱ�Ĵ���
#define PCF8563_DAY				0x05				//�ռĴ���
#define PCF8563_WEK				0x06				//���ڼĴ���
#define PCF8563_MON				0x07				//�¡����ͼĴ���
#define PCF8563_YEA				0x08				//��Ĵ���
#define PCF8563_ALM_MIN			0x09				//���ӱ���
#define PCF8563_ALM_HOU			0x0A				//Сʱ����
#define PCF8563_AIM_DAY			0x0B				//�ձ���
#define PCF8563_ALM_WEK			0x0C				//���ڱ���

//-------------PCF8563������-----------------
#define PCF8563_STOP			0x20
#define PCF8563_START			0x00

//----------------��������---------------------
void clear(uint8 *p, uint8 num);
void pcf8563_init(void);
void updata_time(void);
void pcf8563_cmd(uint8 ctrl_cmd);
void pcf8536_write_p(uint8 addr,uint8 *p,uint8 num);
void pcf8536_write(uint8 addr,uint8 data);
void pcf8563_start(void);
void pcf8563_stop(void);
void pcf8563_time_set(uint8 *time_list);
void pcf8563_time_get(uint8 *time_list);

#if PCF8563_TEST == 1
void pcf8563_test(void);
#endif
//----------------ȫ�ֱ�����-------------------
extern uint8 write_buff[8];
extern uint8 read_buff[8];
extern uint8 time_tmp[];
extern uint8 data_tmp[];
extern uint8 *week_tmp;
extern uint8 time[7];

#endif
