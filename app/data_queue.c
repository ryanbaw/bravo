#include "config.h"

// ---------------�������----------------------
DataQueue		data_queue;
DataBuf			data_buf;

void data_buf_init(void)							//���ݻ�������ʼ����ֻ��һ֡���ݣ�
{
	memset(&data_buf, 0x00, sizeof(DataBuf));

	data_buf.data.data_code = DATA_CODE;			//DATA_CODE=0x55
	data_buf.data.data_size = DATA_MAX_SIZE;		//DATA_MAX_SIZE=24,ȡ��6�����ݣ�4�ֽڱ�ʾһ�����ݣ�
	data_buf.data.data_end  = DATA_END;				//DATA_END=0x0A('\n')
}

void data_queue_init(void)							//���ݶ��г�ʼ������֡���ݣ�
{
	memset(&data_queue, 0x00, sizeof(DataQueue));
}

void data_queue_add_data(Data* pData)				//�����ݶ��������һ֡����
{
	uint8 i;

	if(data_queue_status() == DATAQUEUE_STATUS_FULL)//������ݶ����������򷵻�
		return;

	for(i = 0; i < DATA_MAX_SIZE; i++)				//�˴���Ϊ����LabVIEW��ϣ���Ϊ���õ���ֹ��Ϊ0x0A
	{
		if(pData->cDatas[i] == 0x0A )
			pData->cDatas[i] = 0x0B;
	}
	memcpy(&(data_queue.data[data_queue.tail]), pData, sizeof(Data));//��һ֡pData�������ݶ��е�β��

	data_buf_init();								// ������ݻ���
	DATAQUEUE_TAIL_ADD();							// ���ݸ�����1
}

void data_queue_remove_data(void)					//�����ݶ�����ȥ��һ֡����
{
	if(data_queue_status() == DATAQUEUE_STATUS_EMPTY)
		return;

	DATAQUEUE_HEAD_ADD();							//ͷ������ƶ������õ�ͬ�ڶ���һ֡����
}

Data* data_queue_headptr_get(void)					//������ݶ���ͷָ��
{
	if(data_queue_status() == DATAQUEUE_STATUS_EMPTY)
		return NULL;

	return (&data_queue.data[data_queue.head]);
}

bool data_queue_status(void)
{
	if(data_queue.tail == data_queue.head)
		return DATAQUEUE_STATUS_EMPTY;
		
	if(data_queue.head == (data_queue.tail + 1) % DATA_MAX_NUM)
		return DATAQUEUE_STATUS_FULL;
		
	else
		return DATAQUEUE_STATUS_HASDATA;
}
