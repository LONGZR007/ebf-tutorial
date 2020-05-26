/****************************************************************************************************
 * ������OLED Ӳ��������ʵ��
 *
 * ���ߣ�������
 *
 * �汾��v1.0.0    ���ڣ�2018-04-25
 *                                                                        �����Ǻ��Ƽ����޹�˾
****************************************************************************************************/

#include "./inc/drv_oled.h"
#include "./inc/drv_oledfont.h"
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/****************************************************************************************************
 * ��������ʼ��SSD1306	
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/
  
void OLED_Init(void)
{
	oled_i2c();    // ��ʼ��ģ�� I2C
	
	OLED_WR_Byte(0xAE,OLED_CMD,0);//--display off
	OLED_WR_Byte(0x00,OLED_CMD,0);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD,0);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD,0);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD,0);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD,0); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD,0);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD,0);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD,0);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD,0);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD,0);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD,0);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD,0);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD5,OLED_CMD,0);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD8,OLED_CMD,0);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD,0);//
	
	OLED_WR_Byte(0xD9,OLED_CMD,0);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD,0);//
	
	OLED_WR_Byte(0xDA,OLED_CMD,0);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD,0);//
	
	OLED_WR_Byte(0xDB,OLED_CMD,0);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD,0);//
	
	OLED_WR_Byte(0x8D,OLED_CMD,0);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD,0);//
	
	OLED_WR_Byte(0xAF,OLED_CMD,0);//--turn on oled panel
	
	OLED_Clear();
}  

static int fd_i2c0;

/****************************************************************************************************
 * ������oled i2c ��ʼ��
 *
 * ������void
 *
 * ���أ�void
****************************************************************************************************/

int oled_i2c(void)
{
	fd_i2c0 = open("/dev/i2c-0", O_RDWR);    // �Զ�д�ķ�ʽ���ļ�
	if (fd_i2c0 < 0)
	{
		printf("open /dev/i2c-0 fail\r\n");
		exit(1);
	}

	if (ioctl(fd_i2c0, I2C_SLAVE, OLED_ADDRESS) < 0)
	{
		printf("fail to set i2c device slave address!\r\n");
		close(fd_i2c0);
		return -1;
	}

	return 0;
}

void OLED_DeInit(void)
{
	close(fd_i2c0);
}

/****************************************************************************************************
 * �������������� / ����
 *
 * ������dat = ����
 * ������cmd = ���͵������ݻ�������
 *
 * ���أ���
****************************************************************************************************/

void OLED_WR_Byte(uint8 dat,uint8 cmd,uint8 aga)
{
	if(cmd)
	{
		Write_IIC_Data(dat, aga);       // ��������
	}
	else 
	{
		Write_IIC_Command(dat);    // ��������
	}
}


/****************************************************************************************************
 *                                        ʹ�� SI2C                                                 * 
****************************************************************************************************/

/****************************************************************************************************
 * ����������д����
 *
 * ������Command = ����
 *
 * ���أ���
****************************************************************************************************/

void Write_IIC_Command(uint8 Command)
{
	int retries;
    uint8 data[2];

    data[0] = 0x00;
    data[1] = Command;

    for (retries = 5; retries; retries--)
    {
        if (write(fd_i2c0, data, 2) == 2)
        {
            // printf("write cmd succeed\r\n");
			return;
        }

		printf("write cmd fail -> (%d time)\r\n", retries);
        usleep(1000 * 10);
    }

    printf("write cmd fail\r\n");
}

/****************************************************************************************************
 * ��������������
 *
 * ������Data = ����
 *
 * ���أ���
****************************************************************************************************/

void Write_IIC_Data(uint8 Data, uint8 aga)
{
	int retries;
    uint8 data[2];

    data[0] = 0x40;
    data[1] = aga ? Data : ~Data;

    for (retries = 5; retries; retries--)
    {
        if (write(fd_i2c0, data, 2) == 2)
        {
			// printf("write data succeed\r\n");
            return;

        }

		printf("write data fail -> (%d time)\r\n", retries);
        usleep(1000 * 10);
    }

	printf("write data fail\r\n");
}

 /* SI2CEND */


	
/* δ�����ĺ���ʵ�� */

//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0,1);		//page0-page1
		OLED_WR_Byte(0x00,0,1);		//low column start address
		OLED_WR_Byte(0x10,0,1);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1,1);
			}
	}
}


//��������

void OLED_Set_Pos(uint8 x, uint8 y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD,0);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD,0);
	OLED_WR_Byte((x&0x0f),OLED_CMD,0); 
}   	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD,1);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD,1);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD,1);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD,1);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD,1);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	uint8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,1);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD,1);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD,1);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA,1); 
	} //������ʾ
}
void OLED_On(void)  
{  
	uint8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD,1);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD,1);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD,1);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA,1); 
	} //������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//aga:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
// aga :1:������ʾ 0������
void OLED_ShowChar(uint8 x,uint8 y,uint8 chr,uint8 Char_Size,uint8 aga)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
	if(x > Max_Column-1)
	{
		x=0;
		y=y+2;
	}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA,aga);
		
		OLED_Set_Pos(x,y+1);
		
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA,aga);
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA,aga);
			
	}
}
//m^n����
u32 oled_pow(uint8 m,uint8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8 x,uint8 y,u32 num,uint8 len,uint8 size2,uint8 aga)
{         	
	uint8 t,temp;
	uint8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2, aga);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2, aga); 
	}
} 

//��ʾһ���ַ��Ŵ�

void OLED_ShowString(uint8 x,uint8 y,uint8 *chr,uint8 Char_Size,uint8 aga)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size,aga);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//��ʾ����

void OLED_ShowCHinese(uint8 x,uint8 y,uint8 no,uint8 aga)
{      			    
	uint8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA,aga);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA,aga);
				adder+=1;
      }					
}

/****************************************************************************************************
 * ��������ʾһ��С��
 *
 * ������x, y:�������
 * ������num����ֵ
 * ������vir:С�������Чλ
 * ������size2�������С
 *
 * ���أ���
****************************************************************************************************/

void OLED_ShowDec(uint8 x,uint8 y,float num,uint8 vir,uint8 size2,uint8 aga)
{
	uint8 xC = 0;    // ѭ������
	float temp_num = num;
	u32 temp_vir = vir;
	
	do
	{
		xC++;          // ��������λ��
	}while((u32)(temp_num /= 10.0));
	
	OLED_ShowNum(x, y, (u32)num, xC, size2, aga);                                  // ��ʾ��������
	
	OLED_ShowChar(x + xC * (size2 / 2), y, '.', size2, aga);                       // ��ʾС����
	
	while(temp_vir--)
	{
		num *= 10.0;          // ��������λ��do
	}
	
	OLED_ShowNum(x + (xC + 1) * (size2 / 2), y, (u32)num, vir, size2, aga);         // ��ʾС������
}

/****************************************************************************************************
 * ��������ʾһ���������Բ�ָ������
 *
 * ������x, y:�������
 * ������num����ֵ
 * ������size2�������С
 *
 * ���أ���
****************************************************************************************************/

void OLED_ShowNum_NL(uint8 x,uint8 y,u32 num, uint8 size2,uint8 aga)
{
	uint8 xC = 0;             // ѭ������
	u32 temp_num = num;    // ��ʱ����
	
	do
	{
		xC++;                              // ��������λ��
	}while((u32)(temp_num /= 10.0));       // δ����
	
	OLED_ShowNum(x, y, num, xC, size2,aga);    // ��ʾ����
}

/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/

void OLED_DrawBMP(uint8 x0, uint8 y0,uint8 x1, uint8 y1,uint8 BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA,1);	    	
	    }
	}
} 

/* End Of File */