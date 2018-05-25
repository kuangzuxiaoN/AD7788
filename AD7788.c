#include "AD7788.h"  

/* ���ڶ�д����������ѡ���Ĵ����Ķ�д������ɺ󣬽ӿڷ��ص�ͨ�żĴ���ִ��д������״̬ */
void AD7788_RegInit(void)
{
	uint8_t wData=0x0; 
	 
	wData = AD7788_WRITE_ENABLE | AD7788_REG_MODE | AD7788_WRITE_REG | AD7788_CH_AIN1P_AIN1N;	//(0x10)дͨ�żĴ�����next operations is write from Mode register.
	AD7788_WriteByte(wData); 
	wData = AD7788_MODE_CONTINUOUS | AD7788_MODE_BIPOLAR | (0x1<<1);			//(0x2)дģʽ�Ĵ���, MR1 must 1
	AD7788_WriteByte(wData); 	//ѡ��ģʽ�Ĵ������м������� 
	 
}

void AD7788_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_AD7788_SCK | RCC_AD7788_DIN | RCC_AD7788_DOUT | RCC_AD7788_CS, ENABLE);
	
	/* ���ü����������IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			/* ��Ϊ����� */  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Pin = PIN_AD7788_SCK;
	GPIO_Init(PORT_AD7788_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_AD7788_DIN;			
	GPIO_Init(PORT_AD7788_DIN, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_AD7788_CS;
	GPIO_Init(PORT_AD7788_CS, &GPIO_InitStructure);
	
	/* ����GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	/* ��Ϊ����� */  	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Pin = PIN_AD7788_DOUT;
	GPIO_Init(PORT_AD7788_DOUT, &GPIO_InitStructure);
	  
	AD7788_SCK_1();	AD7788_CS_1();		/*����SCLKΪ1�� CSΪ��*/
	
	AD7788_RegInit();
	
	printf("[%s][%d]: AD7788 Initial Success ! \r\n", __FUNCTION__, __LINE__);
} 

/*
*********************************************************************************************************
*	�� �� ��: AD7788_Write8Bit
*	����˵��: ��SPI���߷���8��bit���ݡ� ��CS���ơ�
*	��    ��: _data : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void AD7788_WriteByte(uint8_t _data)
{
	uint8_t i;   
	uint8_t temp=0x80; 
	
	AD7788_CS_0();
	
	for(i = 0; i < 8; i++)
	{ 
		if ((_data & temp) == 0) 
		{
			AD7788_DI_0();
		}
		else
		{
			AD7788_DI_1();
		}
		AD7788_SCK_0();		//�͵�ƽװ������
//		Delay_us(1);
		AD7788_SCK_1();		//�ߵ�ƽ���
//		Delay_us(1);		
		temp = temp >> 1;
	}
	
	AD7788_CS_1(); 
}


void AD7788_ReadFromReg(uint8_t* dataBuf, uint8_t nByte)
{
	uint8_t i,j;  
	uint8_t temp = 0; 

	AD7788_DI_1();
	AD7788_CS_0();
	AD7788_DO_1();
	
	for(j=0; j<nByte; j++)
	{
		for (i = 0; i < 8; i++)
		{
			AD7788_SCK_0();
			if (AD7788_DO_IS_LOW())
			{
				temp=temp<<1;  
			}else{
				temp=temp<<1; 
				temp = temp + 0x01;
			}
//			Delay_us(1);
			AD7788_SCK_1();
//			Delay_us(1);	 
		}  
		dataBuf[j] = temp;
	}
	
	return ;
}
  

uint16_t AD7788_ReadData(void)
{ 
	uint8_t wData=0x0; 
	uint8_t dataBuf[2]={0x0};
  
	wData = AD7788_WRITE_ENABLE | AD7788_REG_STATUS | AD7788_READ_REG | AD7788_CH_AIN1P_AIN1N;	//(0x08)дͨ�żĴ�����next operations is read from Status register.
	AD7788_WriteByte(wData);	
	AD7788_ReadFromReg(dataBuf, 1);																								//��״̬�Ĵ��� 
	
	while((dataBuf[0] & 0x80) == 0x80)					//��������������ֱ��DRDYλ����ʹ�� ���͵�ƽʹ�ܣ�   
	{
		AD7788_WriteByte(wData);	
		AD7788_ReadFromReg(dataBuf, 1);																								//��״̬�Ĵ��� 
	}  
	
	wData = AD7788_WRITE_ENABLE | AD7788_REG_DATA | AD7788_READ_REG | AD7788_CH_AIN1P_AIN1N;	//(0x38)дͨ�żĴ�����next operations is read from Status register.
	AD7788_WriteByte(wData);
	 
	AD7788_CS_0(); 
	AD7788_ReadFromReg(dataBuf, 2);																								//��״̬�Ĵ��� 
	AD7788_CS_1(); 
	
	return ((dataBuf[0] << 8) | dataBuf[1]);
}   








