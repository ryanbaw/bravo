#ifndef __CMDQUEUE_H_
#define __CMDQUEUE_H_

#define CMD_MAX_NUM					4				//�����������֡����
#define CMD_MAX_NUM_MASK			0x03			//��������
#define PARA_MAX_SIZE				16				//����������

#define FRAME_BEGIN_FLAG 			0xAA			//֡��ʼ���

#define	CMD_RCV_BUF_STATUS_EMPTY	0x00
#define	CMD_RCV_BUF_STATUS_RCVING	0x01
#define	CMD_RCV_BUF_STATUS_READY	0x02

#define CMD_QUEUE_STATUS_FULL		0x01
#define CMD_QUEUE_STATUS_EMPTY		0x02
#define CMD_QUEUE_STATUS_HASDATA	0x00

#define CMDQUEUE_TAIL_ADD();		{cmd_queue.tail = (cmd_queue.tail + 1) & CMD_MAX_NUM_MASK;}
#define CMDQUEUE_HEAD_ADD(); 		{cmd_queue.head = (cmd_queue.head + 1) & CMD_MAX_NUM_MASK;}

typedef struct
{
	uint8 cmd_code;						//�����������ݴ����ǣ�0x55Ϊ����
	uint8 para_size;					//��������
	union
	{
		uint8 para[PARA_MAX_SIZE];		//����������
		struct
		{
			uint8 n;					
			float fPara;
		};
		struct
		{
			uint16 addr;
			union
			{
				float fData;
				long  lData;
				uint16 nData[2];
			};
		};
	};
}Cmd;

typedef struct
{
	uint8 status;
	uint8 ptr;
	Cmd cmd;
}CmdRcvBuf;

typedef struct
{
	uint8 head;
	uint8 tail;
	Cmd cmd[CMD_MAX_NUM];
}CmdQueue;


extern CmdQueue		cmd_queue;
extern CmdRcvBuf	cmd_rcvbuf;

void cmd_rcvbuf_init(void);
void cmd_rcvbuf_add_char(uint8 c);

void cmd_queue_init(void);
void cmd_queue_add_cmd(Cmd* pCmd);
void cmd_queue_remove_cmd(void);
Cmd* cmd_queue_headprt_get(void);
bool cmd_queue_status(void);

#endif
