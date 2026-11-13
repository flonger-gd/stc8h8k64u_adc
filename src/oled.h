#ifndef __OLED_H
#define __OLED_H


sbit OLED_SCL = P3^2;//SCL
sbit OLED_SDA = P3^4;//SDA
sbit OLED_RES = P3^6;//RES
sbit OLED_DC  = P3^7;//DC
sbit OLED_CS  = P3^5;//CS


//【OLED控制用函数】
void OLED_WriteCmd(u8 cmd);
void OLED_WriteDat(u8 dat);
void OLED_Init(void);
void OLED_ClearBuf(u8 dat); //用dat来清屏
void OLED_Flush(); //最后使用该函数才会真正绘图
//【画图 x:0~127 y:0~63】
void OLED_SetPixel(u8 x, u8 y);
void OLED_DrawLine(u8 x0,u8 y0, u8 x1,u8 y1);
void OLED_DrawBMP(u8 x,u8 y,u8 sizex, u8 sizey,u8 BMP[]);
//【文字 x:0~127 y:0~7】
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr);
void OLED_ShowChar8x16(u8 x,u8 y,u8 chr);
void OLED_ShowNum6x8(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowNum8x16(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowString6x8(u8 x,u8 y,u8 *chr);
void OLED_ShowString8x16(u8 x,u8 y,u8 *chr);
void OLED_ShowChinese16x16(u8 x,u8 y,u8 no);
//【OLED扩展函数】
void OLED_ColorTurn(u8 i);//反显函数
void OLED_DisplayTurn(u8 i);//屏幕旋转180度   
void OLED_Display_On(void);//开启OLED显示     
void OLED_Display_Off(void);//关闭OLED显示
#endif
