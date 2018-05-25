#include "AD7792.h"  

void AD7792_Reset(void)			//AD7792 Reset
{
	uint8_t ResetTime=32;
	
	AD7792_SCK_1();	
	AD7792_CS_0();
	AD7792_DI_1();	
	
	while(ResetTime--) 			//to keep DIN=1 for 32 sclock to reset the part
	{
		Delay_us(1);
		AD7792_SCK_0();	
		Delay_us(1);
		AD7792_SCK_1();	
	}
	AD7792_CS_1();
	
	printf("AD7792 Reset\r\n");
}

/* ���ڶ�д����������ѡ���Ĵ����Ķ�д������ɺ󣬽ӿڷ��ص�ͨ�żĴ���ִ��д������״̬ */
void AD7792_RegInit(void)
{
	unsigned char DataRead[3];
	
	AD7792_Reset();
	printf("AD7792 Register Initial!\r\n"); 
	
	AD7792_Write1Byte(0x10); 	//дʹ�ܣ�д���üĴ���write to Communication register.The next step is writing to Configuration register. 
//	AD7792_Write2Byte(0x1010);//������ ��������0-2V5 ����ģʽ �ⲿ��׼��ѹ
	AD7792_Write2Byte(0x0010);//˫���� ��������0-2V5 ����ģʽ �ⲿ��׼��ѹ
		
	AD7792_Write1Byte(0x08);	//дʹ�ܣ�write to Communication register.The next step is writing to Mode register. 
	AD7792_Write2Byte(0x200A);
	
	AD7792_Write1Byte(0x40);	//��ʹ�ܣ�write to Communication register.The next step is to read from Status register.
	AD7792_ReadFromReg(DataRead, 1);	
	while((DataRead[0]&0x80)==0x80)			//wait for the end of convertion by polling the status register RDY bit 
	{
	  
		AD7792_Write1Byte(0x40); 
		AD7792_ReadFromReg(DataRead, 1);	
	}
	  
	AD7792_Write1Byte(0x58);//write to Communication register.The next step is to read from Data register.
	AD7792_ReadFromReg(DataRead, 2);	
	printf("Data : 0x%02x, 0x%02x\r\n",DataRead[0],DataRead[1]); 
}
	
void AD7792_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_AD7792_SCK | RCC_AD7792_DIN | RCC_AD7792_DOUT | RCC_AD7792_CS, ENABLE);
	
	/* ���ü����������IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Pin = PIN_AD7792_SCK;
	GPIO_Init(PORT_AD7792_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_AD7792_DIN;			/*������������Ϊ���룬���ڴӻ�����Ϊ�������˴˴����ó����*/
	GPIO_Init(PORT_AD7792_DIN, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_AD7792_CS;
	GPIO_Init(PORT_AD7792_CS, &GPIO_InitStructure);
	
	/* ����GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* ��Ϊ����� */  	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Pin = PIN_AD7792_DOUT;
	GPIO_Init(PORT_AD7792_DOUT, &GPIO_InitStructure);
	
	AD7792_SCK_1();	AD7792_CS_1();		/*����SCLKΪ1�� CSΪ��*/
	AD7792_Reset();
	AD7792_RegInit();
} 

/*
*********************************************************************************************************
*	�� �� ��: AD7792_Write1Byte
*	����˵��: ��SPI���߷���8��bit���ݡ� ��CS���ơ�
*	��    ��: _data : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void AD7792_Write1Byte(uint8_t _data)
{
	uint8_t i;  
	unsigned char temp = 0x80;
	
	AD7792_CS_0();
	for(i = 0; i < 8; i++)
	{
		if ((_data & temp) == 0)
		{
			AD7792_DI_0();
		}
		else
		{
			AD7792_DI_1();
		}
		AD7792_SCK_0();
		Delay_us(1);
		AD7792_SCK_1();	
		Delay_us(1);	
		temp = temp >> 1;
	} 
	AD7792_CS_1();
}

void AD7792_Write2Byte(uint16_t _data)
{
	uint8_t i;  
	uint16_t temp = 0x8000;
	
	AD7792_CS_0();
	for(i = 0; i < 16; i++)
	{
		if ((_data & temp) == 0)
		{
			AD7792_DI_0();
		}
		else
		{
			AD7792_DI_1();
		}
		AD7792_SCK_0();
		Delay_us(1);
		AD7792_SCK_1();	
		Delay_us(1);	
		temp = temp >> 1;
	} 
	AD7792_CS_1();
}

/*
*********************************************************************************************************
*	�� �� ��: AD7792_ReadFromReg
*	����˵��: д��n���ֽڡ���CS����
*	��    ��: *DataRead ����Ҫд����������飬nByte �����鳤��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD7792_ReadFromReg(unsigned char *DataRead, unsigned char nByte) // nByte is the number of bytes which need to be read
{
	uint8_t i,j;
  unsigned char temp = 0;
			
	AD7792_DI_1();
	AD7792_CS_0();
	AD7792_DO_1();  

	for(i=0; i<nByte; i++)
	{
		for(j=0; j<8; j++)
		{
				AD7792_SCK_0();
				if(AD7792_DO_IS_LOW())
				{
					temp=temp<<1;
				}else
				{
					temp=temp<<1;
					temp=temp+0x01;
				}
				
				Delay_us(1);
				AD7792_SCK_1();	
				Delay_us(1);
		}
		DataRead[i]=temp; 
	}
	
	AD7792_CS_1();
}


uint16_t AD7792_ReadData(void)
{ 
	unsigned char DataRead[2]={0x0};   
	
	AD7792_Write1Byte(0x08);	//дʹ�ܣ�write to Communication register.The next step is writing to Mode register. 
	AD7792_Write2Byte(0x200A);
	
	AD7792_Write1Byte(0x40);	//��ʹ�ܣ�write to Communication register.The next step is to read from Status register.
	AD7792_ReadFromReg(DataRead, 1);	
	while((DataRead[0]&0x80)==0x80)			//wait for the end of convertion by polling the status register RDY bit 
	{
	  
		AD7792_Write1Byte(0x40); 
		AD7792_ReadFromReg(DataRead, 1);	
	}
	  
	AD7792_Write1Byte(0x58);//�����ݼĴ���write to Communication register.The next step is to read from Data register.
	AD7792_ReadFromReg(DataRead, 2);	
	//printf("Data : 0x%02x, 0x%02x\r\n",DataRead[0],DataRead[1]); 
	
	return ((DataRead[0] << 8) | DataRead[1]); 
}








