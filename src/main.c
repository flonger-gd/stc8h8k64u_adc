// 在此添加用户头文件包含
#include "config.h"                     //默认已包含stdio.h、intrins.h等头文件


// 在此添加用户全局变量定义、用户宏定义以及函数声明


void test()  
{
	static u16 i = 0;
	OLED_ClearBuf(0);
	OLED_ShowString6x8(0,0,"FPS:");
	OLED_ShowNum6x8(32,0,i,4);
	OLED_ShowChinese16x16(0,2,0);
	OLED_ShowChinese16x16(16,2,1);
	OLED_ShowChinese16x16(32,2,2);
	OLED_ShowChinese16x16(48,2,3);
	OLED_ShowChinese16x16(64,2,4);
	OLED_Flush();
	//delay_ms(500);
	i++;
}



////////////////////////////////////////
// 项目主函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void main(void)
{
	//【注意】stc8hhk64u 40MHz

    // 在此添加用户主函数初始化代码
    SYS_Init();

    // 在此添加主函数中运行一次的用户代码
	OLED_ClearBuf(0);

    while (1)
    {
		//test();
		Get_WaveData();
		Analyse_Data();
		Plot_Wave();
		Adc_Keydown();
    }
}

////////////////////////////////////////
// 系统初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void SYS_Init(void)
{
    P_SW2 |= 0x80;	//使能访问扩展XFR

    // 在此添加用户预初始化代码
	P1M0 = 0x00;
	P1M1 = 0xFF;
	P2M0 = 0xFF;  
	P2M1 = 0x00;
	P20 = 1;
	P21 = 1;
	P3M0 = 0xFF;  
	P3M1 = 0x00;
	//
    SPI_Init();		//SPI初始化
	OLED_Init();
    ADC_Init(); 	//ADC初始化

    EA = 1;			//使能全局中断
}

////////////////////////////////////////
// 微秒延时函数
// 入口参数: us (设置延时的微秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_us(uint16_t us)
{
    do
    {
        NOP(30);	//(MAIN_Fosc + 500000) / 1000000 - 10
    } while (--us);
}


////////////////////////////////////////
// 毫秒延时函数
// 入口参数: ms (设置延时的毫秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_ms(uint16_t ms)
{
    uint16_t i;

    do
    {
        i = MAIN_Fosc / 10000;
        while (--i);
    } while (--ms);
}

