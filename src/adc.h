#ifndef __ADC_H__
#define __ADC_H__

// 在此添加用户宏定义


// 在此添加用户外部函数和外部变量声明
void ADC_Init(void);
uint16_t ADC_Convert(uint8_t ch);
//
void Get_WaveData(); //采样ADC
void Analyse_Data(); //分析采样数据
void Plot_Wave(); //绘制波形
void Adc_Keydown();


#endif
