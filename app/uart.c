// AVR application builder : 2010-9-2 ���� 12:37:32
// Target : M128
// Crystal: 8.0000Mhz

#include "config.h"

SiocirQueue tx_buf;

static void tx_buf_init(void)							//���ͻ�������ʼ��
{
	tx_buf.head 	 = 0;
	tx_buf.tail  	 = 0;
	tx_buf.disable   = DISABLE;							//������������
}

static bool tx_buf_status(void)							//�գ�2������1��������0
{
	if(tx_buf.tail == tx_buf.head)
		return TXBUF_IS_EMPTY;							//��ⷢ�ͻ������Ƿ�Ϊ�գ�2Ϊ�գ�0Ϊ�ǿ�
	
	if(((tx_buf.tail + 1) & BIT_MASK) == tx_buf.head)
		return TXBUF_IS_FULL;							//��ⷢ�ͻ������Ƿ�������1Ϊ����0Ϊ����
	
	else
		return 0;
}

// UART1 initialize
// desired baud rate: 9600
// actual: baud rate: 9615 (0.2%)
static void uart_set_baudrate(uint32 baudrate)
{
	uint32 tmp;
	tmp    = (F_CPU/baudrate) / 16 - 1;					//F_CPU was defined in Makefile

	UBRR1L = (uint8)tmp;						//set baud rate hi
	UBRR1H = (uint8)(tmp>>8);					//set baud rate lo
}

void uart_init(void)
{
	DDRE   |=  (1<<TXD1);
	DDRE   &= ~(1<<RXD1);
	UCSR1B  = 0x00; 									//disable while setting baud rate
	UCSR1A  = 0x00;
	UCSR1C |= _BV(UCSZ11) | _BV(UCSZ10);				//8λ���ݣ�������9600��8N1,�޼���
	UCSR1B |= _BV(RXEN1)  | _BV(TXEN1) | _BV(TXCIE1) | _BV(RXCIE1);//ʹ�ܷ��ͣ����գ������жϣ������ж�  
	
	uart_set_baudrate(SYS_BAUDRATE);					//���ò����ʣ�9600
	tx_buf_init();										//��ս��ջ�����
}

//��UARTдһ�ֽ�
void uart_putchar(unsigned char c)
{
    if(c == '\n')
	{
        uart_putchar(0x0D);								//�������һ�������⣬�������
	}													//����0Aʱ���ȷ���0DȻ���ٷ���0A��
	//else
    {	loop_until_bit_is_set(UCSR1A, UDRE1);				//�൱�ڷ����������ֽڣ�������
		UDR1 = c;
	}
}

//��UART��һ�ֽ�
int uart_getchar(void)
{
    loop_until_bit_is_set(UCSR1A, RXC1);
    return UDR1;
}

void uart_putenter(void)
{
	uart_putchar(0x0A);
	uart_putchar(0x0D);
}

void uart_putstring(unsigned char *str)							//���ڼ򵥷�ʽ�ķ����ַ�������
{
	unsigned char *pbuf = (unsigned char *)str;
	
	if(pbuf != NULL)
	{
		while(*pbuf)
		{
			uart_putchar(*pbuf++);
		}
	}
}

void kprintf(uint8 *fmt,...)
{
	va_list ap;
	uint8 string[64];
	va_start(ap, fmt);
	vsprintf(string, fmt, ap);
	uart_putstring(string);
	va_end(ap);
}
void uart_putnstring(uint8 *p, uint8 len) 		//���ڼ򵥷�ʽ�ķ���ָ�������ַ�������
{
	uint8 *pbuf = p;
	
	if (!len)
		return;

	for(; len > 0; len--)
	{
		uart_putchar(*pbuf++);
	}
}

void com_printf(const int8* fmt, ...)  
{  
    const int8* s;  
    int d;  
    int8 buf[32]; 
	
    va_list ap;  
    va_start(ap, fmt);   								// ��apָ��fmt�����ɱ�����ĵ�һ��?��һ������  
    while (*fmt)  
    {  
        if (*fmt != '%')  
        {  
            uart_putchar(*fmt++);   					// ��������  
            continue;     
        } 
		if(*fmt == '%')
		{
			fmt++;
		}
        switch (*fmt) 								// next %  
        {  
			case 's':  
				s = va_arg(ap, const uint8*); 			// ��apָ����ת��uint8*�ͣ�������֮  
				for (; *s; s++)  
				{
					uart_putchar(*s);  
				}
				break;  
				
			case 'x':  
				d = va_arg(ap,int);     				// ��apָ����ת��int�ͣ�������֮  
				itoa(d, buf, 16);         				// ������d��16����ת��buf��  
				for (s = buf; *s; s++) 
				{
					uart_putchar(*s); 
				}
				break;
				
			case 'd':  
				d = va_arg(ap,int);  
				itoa(d, buf, 10);         				// ������d��10����ת��buf��  
				for (s = buf; *s; s++)  
                {
					uart_putchar(*s);  
				}
				break; 
				
			default:  
				uart_putchar(*fmt);  
				break;  
		}  
        fmt++;  
    }  
    va_end(ap);  
} 

void com_putchar(uint8 x)						
{
	if(tx_buf_status() == TXBUF_IS_FULL)				//�����ͻ�����������ֱ�ӷ���
		return;											//��ֱ�ӷ��ػ���������ȴ�������������

	TXC_DIS();											//�رշ�������ж�
	if(tx_buf.disable == DISABLE)						//������ͻ������ǲ����õ�
	{
		while(!(UCSR1A & (1<<UDRE1))); 					//����������ݼĴ�����Ϊ����ȴ�
		UDR1 = x;										//�ѵ�ǰҪ���͵������ȷ���ȥ
		tx_buf.disable = ENABLE;						//�÷��ͻ���������
	}
	else
	{
		tx_buf.buf[tx_buf.tail] = x;					//�����͵����ݷŵ�������β��
		tx_buf.tail = (tx_buf.tail + 1) & BIT_MASK;		//�ƶ�������β��ָ��
	}
	TXC_EN();											//�򿪷�������ж�
}

void com_putstring(uint8 *p, uint8 len) 
{
	uint8 i;
	if (!len)
		return;

	for(i = 0; i < len; i++)
	{
		com_putchar(p[i]);
	}
}

void com_putenter(void)
{
	com_putchar('\n');
	com_putchar('\r');
}

void com_putcommand(Cmd *pCmd)
{
	com_putchar(FRAME_HEAD);
	com_putstring((uint8*)pCmd, pCmd->para_size + 2);
}

void com_putdata(Data *pData)
{
	com_putchar(FRAME_HEAD);
	com_putstring((uint8*)pData, DATA_LENGTH);
}

void com_put_ackcommand(void)
{
	Cmd cmd;
	cmd.cmd_code 	= 0x01;
	cmd.para_size 	= 0;
	com_putcommand(&cmd);
}


ISR(USART1_RX_vect)									//��������жϷ����ӳ���
{
	uint8 status = UCSR1A;					//�ж�״̬
	uint8 data   = UDR1;					//��ȡ��������

	if((status & (FRAMING_ERROR | DATA_OVERRUN)) == 0)
	{
		cmd_rcvbuf_add_char(data);
	}
}

ISR(USART1_TX_vect)									//��������жϷ����ӳ���
{
	if (tx_buf_status() != TXBUF_IS_EMPTY)			//�жϷ��ͻ������Ƿ�Ϊ�գ���Ϊ�����ͻ���������
	{
		UDR1 = tx_buf.buf[tx_buf.head];				//�ӷ��ͻ�������ͷ����ʼ����
		tx_buf.head = (tx_buf.head+1) & BIT_MASK;	//����ͷ��λ��
	}
	else 											//������disable=1
	{
		tx_buf.disable = DISABLE;					//��λ������������
	}
}
//*/