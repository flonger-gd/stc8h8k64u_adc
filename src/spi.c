#include "config.h"

// 在此添加用户全局变量定义、用户宏定义以及函数声明
  

////////////////////////////////////////
// SPI初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void SPI_Init(void)
{
    // 在此添加用户初始化代码
    MODIFY_REG(P_SW1, BIT3 | BIT2, ((3) << 2));	//选择SPI数据口: SS(P3.5), MOSI(P3.4), MISO(P3.3), SCLK(P3.2)

    SET_REG_BIT(SPCTL, BIT4);                   //设置SPI为主机模式
    SET_REG_BIT(SPCTL, BIT7);                   //忽略SS脚
    CLR_REG_BIT(SPCTL, BIT5);                   //设置SPI数据顺序为MSB (高位在前)
    MODIFY_REG(SPCTL, BIT3 | BIT2, ((0) << 2)); //设置SPI工作模式0 (CPOL=0, CPHA=0)
    MODIFY_REG(SPCTL, BIT1 | BIT0, ((0) << 0)); //设置SPI时钟分频

    SET_REG_BIT(SPCTL, BIT6);                   //使能SPI功能
}

////////////////////////////////////////
// SPI主机模式发送字节函数
// 入口参数: dat (待发送的字节数据)
// 函数返回: 无
////////////////////////////////////////
void SPI_WriteByte(uint8_t dat)
{
    SPDAT = dat;                  			//触发主机发送数据
    while (!READ_REG_BIT(SPSTAT, BIT7));    //等待发送完成
    SET_REG_BIT(SPSTAT, BIT7 | BIT6);       //清除中断标志
}

////////////////////////////////////////
// SPI主机模式读取字节函数
// 入口参数: 无
// 函数返回: 读取的字节数据
////////////////////////////////////////
uint8_t SPI_ReadByte(void)
{
    SPDAT = 0xff;                 			//触发主机读取数据（主机发送时钟信号）
    while (!READ_REG_BIT(SPSTAT, BIT7));    //等待读取完成
    SET_REG_BIT(SPSTAT, BIT7 | BIT6);       //清除中断标志
    return SPDAT;
}


