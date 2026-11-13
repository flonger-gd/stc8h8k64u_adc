#include "config.h"
#include "ziku.h"

xdata u8 oled_buf[8][128]; 			   

//【OLED控制函数】////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OLED_WriteCmd(u8 cmd){
	OLED_CS = 0;			  
	OLED_DC = 0;
	//硬件spi
	SPI_WriteByte(cmd);   	  
}
void OLED_WriteDat(u8 dat){
	OLED_CS = 0;			  
	OLED_DC = 1;
	//硬件spi
	SPI_WriteByte(dat);   	  
}

//初始化				    
void OLED_Init(void)
{
	//OLED_Reset();
	OLED_RES = 0;
	delay_ms(20);
	OLED_RES = 1;
	delay_ms(20);
	
	OLED_WriteCmd(0xAE); /*display off*/ 
	OLED_WriteCmd(0x02); /*set lower column address*/ 
	OLED_WriteCmd(0x10); /*set higher column address*/ 
	OLED_WriteCmd(0x40); /*set display start line*/ 
	OLED_WriteCmd(0xB0); /*set page address*/
	OLED_WriteCmd(0x81); /*contract control*/ 
	OLED_WriteCmd(0xcf); /*128*/ 
	OLED_WriteCmd(0xA1); /*set segment remap*/ 
	OLED_WriteCmd(0xA6); /*normal / reverse*/ 
	OLED_WriteCmd(0xA8); /*multiplex ratio*/ 
	OLED_WriteCmd(0x3F); /*duty = 1/64*/ 
	OLED_WriteCmd(0xad); /*set charge pump enable*/ 
	OLED_WriteCmd(0x8b); /* 0x8B 内供 VCC */ 
	OLED_WriteCmd(0x33); /*0X30---0X33 set VPP 9V */ 
	OLED_WriteCmd(0xC8); /*Com scan direction*/ 
	OLED_WriteCmd(0xD3); /*set display offset*/ 
	OLED_WriteCmd(0x00); /* 0x20 */ 
	OLED_WriteCmd(0xD5); /*set osc division*/ 
	OLED_WriteCmd(0x80); 
	OLED_WriteCmd(0xD9); /*set pre-charge period*/ 
	OLED_WriteCmd(0x1f); /*0x22*/ 
	OLED_WriteCmd(0xDA); /*set COM pins*/ 
	OLED_WriteCmd(0x12); 
	OLED_WriteCmd(0xdb); /*set vcomh*/ 
	OLED_WriteCmd(0x40);
	OLED_WriteCmd(0xAF); /*display ON*/
}

void OLED_ClearBuf(u8 dat){
	u8 i,j;
	for(i=0;i<8;i++)
		for(j=0;j<128;j++)
			oled_buf[i][j] = dat;
}
void OLED_Flush(){
	u8 p,col;
	for(p=0;p<8;p++){
		OLED_WriteCmd(0xb0+p);
		OLED_WriteCmd(0x02);
		OLED_WriteCmd(0x10);
		for(col=0;col<128;col++){
			OLED_WriteDat(oled_buf[p][col]);
		}
	}
}

//【画图 x:0~127 y:0~63】////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OLED_SetPixel(u8 x, u8 y){
	if(x>127 || y>63)return;
	oled_buf[y>>3][x] |= (1<<(y&7));
}

void OLED_DrawLine(u8 x0,u8 y0, u8 x1,u8 y1){
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        OLED_SetPixel(x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }	
}

//显示图片
//x,y显示坐标
//sizex,sizey,图片长宽
//BMP：要显示的图片
void OLED_DrawBMP(u8 x,u8 y,u8 sizex, u8 sizey,u8 BMP[])
{
    u8 page;
    u8 col;
    u16 idx = 0;
    u8 pages = sizey >> 3;     /* sizey / 8 = 占用页数 */

    /* 参数保护 */
    if (x + sizex > 128)  return;
    if (y + pages > 8)    return;

    for (page = 0; page < pages; page++) {
        for (col = 0; col < sizex; col++) {
            oled_buf[y + page][x + col] = BMP[idx++];
        }
    }
}

//【文字 x:0~127 y:0~7】///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				 
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr)
{
	u8 i,c;
	c = chr-' ';//得到偏移后的值
	for(i=0;i<6;i++){
		oled_buf[y][x+i] = asc2_0806[c][i];
	}      	
}
void OLED_ShowChar8x16(u8 x,u8 y,u8 chr)
{
	u8 i,c;
	c = chr-' ';//得到偏移后的值
	for(i=0;i<16;i++){
		if(i<8){
			oled_buf[y][x+i%8] = asc2_1608[c][i];
		}else{
			oled_buf[y+1][x+i%8] = asc2_1608[c][i];
		}
	}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小		  
void OLED_ShowNum6x8(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t,temp,m=0;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar6x8(x+6*t,y,' ');
				continue;
			}else enshow=1;
		}
	 	OLED_ShowChar6x8(x+6*t,y,temp+'0');
	}
}
void OLED_ShowNum8x16(u8 x,u8 y,u32 num,u8 len)
{
	u8 t,temp,m=0;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar8x16(x+8*t,y,' ');
				continue;
			}else enshow=1;
		}
	 	OLED_ShowChar8x16(x+8*t,y,temp+'0');
	}
}
//显示一个字符号串
void OLED_ShowString6x8(u8 x,u8 y,u8 *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar6x8(x,y,chr[j++]);
		x+=6;
	}
}
void OLED_ShowString8x16(u8 x,u8 y,u8 *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar8x16(x,y,chr[j++]);
		x+=8;
	}
}
//显示汉字
void OLED_ShowChinese16x16(u8 x,u8 y,u8 no)
{
    u8 i;
    u8 c;

    /* 参数保护 */
    if (x > 112) return;        /* 16像素宽，x最大112 */
    if (y > 6)  return;         /* 占2页，y最大6 */

    for (i = 0; i < 16; i++) {
        c = (u8)i + x;
        if (c >= 128) break;    /* 越界保护 */
        oled_buf[y]    [c] = Hzk[no][i];       /* 上半8行 */
        oled_buf[y + 1][c] = Hzk[no][i + 16];  /* 下半8行 */
    }				
}

//【OLED扩展函数】////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//反显函数
void OLED_ColorTurn(u8 i){
	if(i==0){
		OLED_WriteCmd(0xA6);//正常显示
	}
	if(i==1){
		OLED_WriteCmd(0xA7);//反色显示
	}
}
//屏幕旋转180度
void OLED_DisplayTurn(u8 i){
	if(i==0){
		OLED_WriteCmd(0xC8);//正常显示
		OLED_WriteCmd(0xA1);
	}
	if(i==1){
		OLED_WriteCmd(0xC0);//反转显示
		OLED_WriteCmd(0xA0);
	}
}  	  
//开启OLED显示    
void OLED_Display_On(void){
	OLED_WriteCmd(0X8D);  //SET DCDC命令
	OLED_WriteCmd(0X14);  //DCDC ON
	OLED_WriteCmd(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void){
	OLED_WriteCmd(0X8D);  //SET DCDC命令
	OLED_WriteCmd(0X10);  //DCDC OFF
	OLED_WriteCmd(0XAE);  //DISPLAY OFF
}		   			 

