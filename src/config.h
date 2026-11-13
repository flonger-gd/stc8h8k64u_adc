#ifndef __CONFIG_H__
#define __CONFIG_H__


// 在此添加用户宏定义
#define __ENCODING              "GB2312" //DO NOT DELETE or MODIFY


#define HIRC                    40000000UL
#define FOSC                    40000000UL
#define SYSCLK                  FOSC
#define MAIN_Fosc               FOSC

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <intrins.h>
// 在此添加用户头文件包含  
#include "stc8h.h"
#include "def.h"
#include "timer.h"
#include "adc.h"
#include "spi.h"
#include "oled.h"

// 在此添加用户外部函数和外部变量声明
void SYS_Init(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);


#endif
