#ifndef __DATAQUEUE_H_
#define __DATAQUEUE_H_

#define DATA_MAX_NUM				4
#define DATA_MAX_NUM_MASK			0x03
#define DATA_MAX_SIZE				24
#define DATA_LENGTH					(DATA_MAX_SIZE+3)		//��ʼ�������+���ݳ���=2+1=3

#define	DATA_CODE					0x55
#define	DATA_END					0x0A

#define DATAQUEUE_STATUS_FULL		1
#define DATAQUEUE_STATUS_EMPTY		2
#define DATAQUEUE_STATUS_HASDATA	0

#define DATAQUEUE_TAIL_ADD();		{data_queue.tail=(data_queue.tail+1)&DATA_MAX_NUM_MASK;} //��β�����һ֡
#define DATAQUEUE_HEAD_ADD();	 	{data_queue.head=(data_queue.head+1)&DATA_MAX_NUM_MASK;} //������ȥ��ͷ��һ֡

//-----------------���ݽṹ��-----------------------
typedef struct
{
	uint8 data_code;					//���ݻ�������룬0x55��ʾ���ݣ�����Ϊ����
	uint8 data_size;					//���ݳ���
	union
	{
		uint8 cDatas[DATA_MAX_SIZE];	//���ݻ�����
		float fDatas[DATA_MAX_SIZE/4];			//���ڸ�����������������6��������������δ�õ�
		struct									//���½ṹ��Ҳδ�õ�~����
		{
			float fCurrent;						//����
			float fVoltage;						//��ѹ
			float fPower;						//����
			float fFlowSpeed;					//����
			float fTorque;						//ת��
			float fRotateSpeed;					//ת��
		};
	};
	uint8 data_end;						//����β
}Data;

typedef struct
{
	uint8 status;						//״̬
	Data data;									//һ֡����
}DataBuf;

typedef struct
{
	uint8 head;							//���ݶ���ͷ
	uint8 tail;							//���ݶ���β
	Data data[DATA_MAX_NUM];					//����Ԫ�ػ���������DATA_MAX_NUM֡����
}DataQueue;

extern DataQueue	data_queue;
extern DataBuf		data_buf;

void  data_buf_init(void);
void  data_queue_init(void);
void  data_queue_add_data(Data* pData);
void  data_queue_remove_data(void);
Data* data_queue_headptr_get(void);
bool  data_queue_status(void);

#endif
