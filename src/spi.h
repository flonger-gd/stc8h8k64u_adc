#ifndef __SPI_H__
#define __SPI_H__

// 在此添加用户宏定义  


// 在此添加用户外部函数和外部变量声明
void SPI_Init(void);
uint8_t SPI_ReadByte(void);
void SPI_WriteByte(uint8_t dat);

#endif
