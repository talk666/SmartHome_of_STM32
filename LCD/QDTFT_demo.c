/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "LCD.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"ȫ�����Ӽ���");
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Һ�����Գ���");

	DisplayButtonUp(15,38,113,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,40,YELLOW,GRAY0,"��ɫ������");

	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,70,BLUE,GRAY0,"������ʾ����");

	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,100,RED,GRAY0,"ͼƬ��ʾ����");
	DelayMs(1500);
}

void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	DelayMs(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+5,RED,GRAY0,Num[i+1]);
	DelayMs(100);
	}
	
}

void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	/*
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"������ʾ����");

	delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,30,RED,GRAY0,"�¶�:");
	Gui_DrawFont_GBK16(16,50,BLUE,GRAY0,"רעҺ������");
	Gui_DrawFont_GBK16(16,70,RED,GRAY0, "ȫ�̼���֧��");
	Gui_DrawFont_GBK16(0,100,BLUE,GRAY0,"Tel:11111111111");
	Gui_DrawFont_GBK16(0,130,RED,GRAY0, "QQ:11111111");	
	delay_ms(1800);	*/
	Gui_DrawFont_GBK24(10,50,RED,GRAY0,"�¶�");
	Gui_DrawFont_GBK24(10,86,RED,GRAY0,"ʪ��");
}

void Color_Test(void)
{
	u8 i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	DelayMs(200);

	while(i--)
	{
		Lcd_Clear(WHITE);
		Lcd_Clear(BLACK);
		Lcd_Clear(RED);
	  	Lcd_Clear(GREEN);
	  	Lcd_Clear(BLUE);
	}		
}

//ȡģ��ʽ ˮƽɨ�� ������ ��λ��ǰ
void showimage(const unsigned char *p) //��ʾ128*160 QQͼƬ
{
  	int i; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //����  
	
  
	Lcd_SetRegion(0,0,127,159);		//��������
	for(i=0;i<128*160;i++)
	 {	
		picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);				
		LCD_WriteData_16Bit(picH<<8|picL);  						
	 }	
		

}
void QDTFT_Test_Demo(void)
{
	LCD_Init();
	LCD_LED_SET;//ͨ��IO���Ʊ�����				
	//Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)
	//Color_Test();//�򵥴�ɫ������
	//Num_Test();//������������
	Font_Test();//��Ӣ����ʾ����		
	//showimage(gImage_qq);//ͼƬ��ʾʾ��
	//delay_ms(1200);
	while(1);
	//LCD_LED_CLR;//IO���Ʊ�����	
	
}
