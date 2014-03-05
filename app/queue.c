#include "config.h"

/*********************************************************************************************************
** ��������: QueueCreate
** ��������: ��ʼ�����ݶ���
** �䡡��: data_buf 	��Ϊ���з���Ĵ洢�ռ��ַ
**         buf_len		��Ϊ���з���Ĵ洢�ռ��С���ֽڣ�
**         ReadEmpty	��Ϊ���ж���ʱ�������
**         WriteFull	��Ϊ����д��ʱ�������
** �䡡��: NOT_OK	:��������
**         QUEUE_OK	:�ɹ�
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 queue_create(void      *data_buf,			/*Ϊ���з���Ļ������洢�ռ��ַ				*/
                  uint32    buf_len,			/*Ϊ���з���Ļ������洢�ռ��С���ֽڣ�		*/
                  ReadEmpty readempty,
                  WriteFull writefull)
{
    DataQueue *queue;
    
    if(data_buf != NULL && buf_len >= (sizeof(DataQueue)))        	/* �жϲ����Ƿ���Ч 		*/
    {
        queue = (DataQueue *)data_buf;			//��ȡ���ݻ�������ַ

        OS_ENTER_CRITICAL();
																	/* ��ʼ���ṹ������ 		*/
        queue->data_len = (buf_len - (uint32)(((DataQueue *)0)->data_buf)) / sizeof(uint8); /* ������п��Դ洢��������Ŀ */
        queue->end = queue->data_buf + queue->data_len;             /* �������ݻ���Ľ�����ַ 	*/
        queue->out = queue->data_buf;								/* �������ݶ�ȡָ�� 		*/
        queue->in  = queue->data_buf;								/* ��������д��ָ�� 		*/
        queue->data_num  = 0;										/* ��ǰ��������Ԫ�ظ��� 	*/
        queue->readempty = readempty;								/* �ص�����					*/
        queue->writefull = writefull;

        OS_EXIT_CRITICAL();

        return QUEUE_OK;
    }
    else
    {
        return NOT_OK;
    }
}


/*********************************************************************************************************
** ��������: QueueRead
** ��������: ��ȡ�����е����ݣ���ȡһ�ֽ�
** �䡡��: ret			:�洢���ص���Ϣ�ĵ�ַ
**         data_buf		:ָ����е�ָ��
** �䡡��: NOT_OK     	����������
**         QUEUE_OK    	���յ���Ϣ
**         QUEUE_EMPTY 	������Ϣ
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 queue_read(uint8 *ret, void *data_buf)					/* ��ȡ�����е����� */
{
    uint8 err;
    DataQueue *queue;

    err = NOT_OK;
    if (data_buf != NULL)                                       /* �����Ƿ���Ч 	*/
    {                                                           /* ��Ч 			*/
        queue = (DataQueue *)data_buf;							/* ��ȡ����ͷ��ַ	*/			
        
        OS_ENTER_CRITICAL();
        
        if (queue->data_num > 0)                                /* �����Ƿ�Ϊ�� 	*/
        {                                                       /* ����         	*/
            *ret = queue->out[0];                               /* ���ݳ��ӵ�ret   	*/
            queue->out++;                                       /* ��������ָ�� 	*/
            if (queue->out >= queue->end)
            {
                queue->out = queue->data_buf;					/* �������ָ����ڶ���β��������ָ��ָ�����ͷ */
            }
            queue->data_num--;                                  /* �������ݼ���      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* ������п�        */
            err = QUEUE_EMPTY;
            if (queue->readempty != NULL)                       /* �����û�������  */
            {
                err = queue->readempty(ret, queue);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** ��������: QueueWrite
** ��������: FIFO��ʽ��������,һ�η���һ���ֽ�
** �䡡��: data_buf 	:ָ����е�ָ��
**         data			:��Ϣ����
** �䡡��: NOT_OK   	:��������
**         QUEUE_FULL	:������
**         QUEUE_OK  	:���ͳɹ�
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE
#define EN_QUEUE_WRITE      0
#endif

#if EN_QUEUE_WRITE > 0

uint8 queue_write(void *data_buf, uint8 data)						/* ��������д����	*/
{
    uint8 err;
    DataQueue *queue;

    err = NOT_OK;
    if (data_buf != NULL)                                           /* �����Ƿ���Ч 	*/
    {
        queue = (DataQueue *)data_buf;								/* ��ȡ�����׵�ַ	*/
        
        OS_ENTER_CRITICAL();
        
        if (queue->data_num < queue->data_len)                      /* ������в���  	*/
        {                                                           /* ����        		*/
            queue->in[0] = data;                                    /* �������    		*/
            queue->in++;                                           	/* �������ָ��		*/
            if (queue->in >= queue->end)							/* ������ָ����ڳ���ָ�� */
            {
                queue->in = queue->data_buf;						/* �����ָ��ָ�����ͷ 	*/
            }
            queue->data_num++;                                      /* ��������    		*/
            err = QUEUE_OK;
        }
        else
        {                                                          	/* ��           	*/
            err = QUEUE_FULL;
            if (queue->writefull != NULL)                           /* �����û������� */
            {
                err = queue->writefull(queue, data, Q_WRITE_MODE);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}
#endif

/*********************************************************************************************************
** ��������: QueueWriteFront
** ��������: LIFO��ʽ��������
** �䡡��: data_buf		:ָ����е�ָ��
**         data			:��Ϣ����
** �䡡��: QUEUE_FULL	:������
**         QUEUE_OK		:���ͳɹ�
				|-------|�ߵ�ַ
				|		|	^
		   end->|-------|	|
				|		|	|
				|		|	|
			in->|-------|	|
				|		|	|
				|		|	|
				|		|	|
				|		|	|
		   out->|-------|	|
				|		|	|
				|		|   |
	  data_buf->|-------|�͵�ַ
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_WRITE_FRONT
#define EN_QUEUE_WRITE_FRONT    0
#endif

#if EN_QUEUE_WRITE_FRONT > 0

uint8 queue_write_front(void *data_buf, uint8 data)						/* �����ݲ��뵽������ 	*/
{
    uint8 err;
    DataQueue *queue;

    err = NOT_OK;
    if (data_buf != NULL)                                        		/* �����Ƿ���Ч 		*/
    {
        queue = (DataQueue *)data_buf;									/* ��ȡ�����׵�ַ		*/
        
        OS_ENTER_CRITICAL();
        
        if (queue->data_num < queue->data_len)                          /* ������в���  		*/
        {                                                               /* ���� 				*/
            queue->out--;                                               /* ��������ָ�� 		*/
            if (queue->out < queue->data_buf)							/* �������ָ��С�ڶ����׵�ַ 	*/
            {
                queue->out = queue->end - 1;							/* ������ָ��ŵ���β֮ǰ�ĵ�ַ */
            }
            queue->out[0] = data;                                       /* �������     		*/
            queue->data_num++;                                          /* ������Ŀ���� 		*/
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           		*/
            err = QUEUE_FULL;
            if (queue->writefull != NULL)                               /* �����û������� 	*/
            {
                err = queue->writefull(queue, data, Q_WRITE_FRONT_MODE);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

#endif

/*********************************************************************************************************
** ��������: Queuedata_num
** ��������: ȡ�õ�ǰ���������ݸ���
** �䡡��: data_buf	:ָ����е�ָ��
** �䡡��: ��Ϣ��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_NDATA
#define EN_QUEUE_NDATA    0
#endif

#if EN_QUEUE_NDATA > 0

uint16 queue_data_num(void *data_buf)
{
    uint16 temp;
    
    temp = 0;                                                   /* ������Ч����0 */
    if (data_buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)data_buf)->data_num;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** ��������: QueueSize
** ��������: ȡ�ö���������
** �䡡��: data_buf:ָ����е�ָ��
** �䡡��: ����������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_SIZE
#define EN_QUEUE_SIZE    0
#endif

#if EN_QUEUE_SIZE > 0

uint16 queue_size(void *data_buf)
{
    uint16 temp;
    
    temp = 0;                                                   /* ������Ч����0 */
    if (data_buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)data_buf)->data_len;
        OS_EXIT_CRITICAL();
    }
    return temp;
}

#endif

/*********************************************************************************************************
** ��������: OSQFlush
** ��������: ��ն���
** �䡡��: data_buf:ָ����е�ָ��
** �䡡��: ��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef EN_QUEUE_FLUSH
#define EN_QUEUE_FLUSH    0
#endif

#if EN_QUEUE_FLUSH > 0

void queue_flush(void *data_buf)
{
    DataQueue *queue;
    
    if (data_buf != NULL)                                           /* �����Ƿ���Ч */
    {                                                               /* ��Ч         */
        queue = (DataQueue *)data_buf;
        OS_ENTER_CRITICAL();
        queue->out = queue->data_buf;
        queue->in  = queue->data_buf;
        queue->data_num = 0;                                         /* ������ĿΪ0 */
        OS_EXIT_CRITICAL();
    }
}

#endif
