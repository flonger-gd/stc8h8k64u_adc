// 在此添加用户头文件包含
#include "config.h"


// 在此添加用户全局变量定义、用户宏定义以及函数声明  
#define ADC_STPCYC              1       //ADC通道选择建立时间
#define ADC_HLDCYC              2       //ADC通道选择保持时间
#define ADC_SMPCYC              15      //ADC通道采样时间
#define SAMPLE_NUM 300	//采样点数
//  
u16 first_pos, last_pos;
u16 v_max, v_min, max_vpp;
u32 freq_hz;
xdata u16 wave_data[SAMPLE_NUM]; //采样的数据
code u16 arr_x_time_per_pixel[] = {2,5,10,20,50,100,200,500,1000,1600};	//采样间隔
u8 index_x_time_per_pixel;
u16 my_count; //防止vpp闪太快，看不清

////////////////////////////////////////
// ADC初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void ADC_Init(void)
{
    MODIFY_REG(ADCCFG, BIT_LN, 0);             						//设置ADC时钟
    SET_REG_BIT(ADCCFG, BIT5);             							//设置ADC结果右对齐(12位结果)
    MODIFY_REG(ADCEXCFG, BIT2 | BIT1 | BIT0, 0);            		//关闭ADC自动重复转换功能

    MODIFY_REG(ADCTIM, BIT7, (((ADC_STPCYC) - 1) << 7));   			//设置ADC通道选择建立时间
    MODIFY_REG(ADCTIM, BIT6 | BIT5, (((ADC_HLDCYC) - 1) << 5));		//设置ADC通道选择保持时间
    MODIFY_REG(ADCTIM, BIT4 | BIT_LN, (((ADC_SMPCYC) - 1) << 0)); 	//设置ADC通道采样时间

    MODIFY_REG(ADC_CONTR, BIT_LN, 0);  //选择ADC通道
    SET_REG_BIT(ADC_CONTR, BIT7);      //使能ADC功能

	//触发定时器
	index_x_time_per_pixel = 2;
    TIMER0_Init(arr_x_time_per_pixel[index_x_time_per_pixel]); //定时器0初始化
}

////////////////////////////////////////
// 获取ADC转换结果函数
// 入口参数: ch (ADC通道选择)
// 函数返回: ADC转换结果
////////////////////////////////////////
uint16_t ADC_Convert(uint8_t ch)
{
    uint16_t res; //定义保存ADC结果的变量

    MODIFY_REG(ADC_CONTR, BIT_LN, ((ch) << 0));	//选择ADC通道
    SET_REG_BIT(ADC_CONTR, BIT6);               //开始ADC转换
    while (!READ_REG_BIT(ADC_CONTR, BIT5));     //等待ADC转换完成
    CLR_REG_BIT(ADC_CONTR, BIT5);               //清除ADC转换完成中断标志
    res = MAKEWORD(ADC_RESL, ADC_RES);          //读取ADC转换结果

    return res; //返回ADC结果
}

//先调用“开始ADC转换”，再调用该函数
u16 Get_Adc_Result()
{
	while (!READ_REG_BIT(ADC_CONTR, BIT5));     //等待ADC转换完成
    CLR_REG_BIT(ADC_CONTR, BIT5);               //清除ADC转换完成中断标志
    return MAKEWORD(ADC_RESL, ADC_RES);          //读取ADC转换结果
}

void Get_WaveData() //采样ADC
{
	u16 i;

	//已在定时器中触发“开始ADC转换”

	//舍弃前2个数据
	for(i=0;i<2;i++)
	{
			Get_Adc_Result();
	}
	//连续采样
	for(i=0;i<SAMPLE_NUM;i++)
	{
			wave_data[i] = Get_Adc_Result();
	}
}
void Analyse_Data() //分析采样数据
{
    uint16 i;
    uint16 tmp; //临时变量
	u16 dacMid; //触发值
    u32 period_us;
    u8  beat_cnt  = 0;       /* 触发点个数（= 周期数+1） */
    first_pos = 0xFFFF;
    last_pos  = 0xFFFF;

	//获取最大最小值
    v_max = 0;
    v_min = 4095;
    for (i = 0; i < SAMPLE_NUM; i++)
    {
        tmp = *(wave_data + i);
        if (tmp > v_max)
            v_max = tmp;
        if (tmp < v_min)
            v_min = tmp;
    }
	//查找触发点first_pos
	dacMid = (v_max+v_min)>>1;//v_min + ((v_max-v_min)>>2);//OLED_ShowNum6x8(0,0,dacMid,5);OLED_Flush();while(1);//test////////////////////
    for (i = 0; i < SAMPLE_NUM; i++)
    {
        //if (*(wave_data + i) >= dacMid && *(wave_data + i + 1) <= dacMid)//下降沿触发
		if (*(wave_data + i) <= dacMid && *(wave_data + i + 1) >= dacMid)//上升沿触发
		{
            if (first_pos == 0xFFFF) first_pos = i;   /* 记第一个 */
            last_pos = i;                             /* 不断更新最后一个 */
            beat_cnt++;
		}
    }
	//计算频率
	/*---------- 4. 触发点不足两个 → 无有效周期 ----------*/
    if (beat_cnt < 2) {
        freq_hz = 0;
        return;
    }
    /*---------- 5. 首尾跨度 ÷ 周期数 = 平均周期 → 频率 ----------*/
    /* arr_x_time_per_pixel[] 的单位必须是"每个采样点多少 us" */
    period_us = (u32)(last_pos - first_pos)
              * (u32)arr_x_time_per_pixel[index_x_time_per_pixel]
              / (u32)(beat_cnt - 1);              /* 除以完整周期数 */

    if (period_us == 0) { freq_hz = 0; return; }

    freq_hz = 1000000UL / period_us;                 /* Hz */
    if (freq_hz > 200000UL) freq_hz = 0;                /* 超 200kHz 视为误判 */
}

void Plot_Wave() //绘制波形  
{
	u16 x, tmp;
	u16 vpp = v_max-v_min;
	u16 scale = vpp / 56;//自动缩放Y轴倍率	//vpp >> 6;//除以64来计算y轴倍数
	scale++;
	//画波形 100x55
	OLED_ClearBuf(0);
	for(x=0;x<100;x++){
		tmp = x+first_pos;if(tmp>SAMPLE_NUM)break;
		OLED_SetPixel(x,63-(wave_data[tmp]-v_min)/scale);
//		OLED_DrawLine(x,63-wave_data[x],x+1,63-wave_data[x+1]); 
	}
	//画电压
	OLED_ShowString6x8(0,0,"v:");
	OLED_ShowNum6x8(12,0,v_min,4);
	OLED_ShowString6x8(36,0,"-");
	OLED_ShowNum6x8(42,0,v_max,4);
	//画vpp
	if(max_vpp <= vpp)max_vpp = vpp;
	if(my_count%30==0){
		max_vpp = 0;
		my_count = 1;
	}else
		my_count++;
	OLED_ShowString6x8(78,0,"vpp:");
	OLED_ShowNum6x8(102,0,max_vpp,4);
	//画每点采样时间间隔
	OLED_ShowNum6x8(100,2,arr_x_time_per_pixel[index_x_time_per_pixel],4);
	OLED_ShowString6x8(100,3,"us");
	//画一屏“参考频率”
	OLED_ShowNum6x8(100,4,1000000UL/arr_x_time_per_pixel[index_x_time_per_pixel]/100,4);
	//画"计算频率"
	if(freq_hz<1000){
		OLED_ShowNum6x8(100,5,freq_hz,4);
		OLED_ShowString6x8(100,6,"Hz");
	}else
	{
		OLED_ShowNum6x8(94,5,freq_hz/1000,2);OLED_ShowString6x8(106,5,".");OLED_ShowNum6x8(112,5,(freq_hz/10)%100,2);if((freq_hz/10)%100 < 10)OLED_ShowString6x8(112,5,"0");
		OLED_ShowString6x8(100,6,"kHz");
	}
	//刷新屏幕
	OLED_Flush();
}

void Adc_Keydown()
{
	if(!P20) //上
	{
		//增加采样时间间隔
		index_x_time_per_pixel++;if(index_x_time_per_pixel>9)index_x_time_per_pixel=9;
		TIMER0_Init(arr_x_time_per_pixel[index_x_time_per_pixel]);
		delay_ms(200);
	}
	else if(!P21) //下
	{
		//减少采样时间间隔
		index_x_time_per_pixel--;if(index_x_time_per_pixel>9)index_x_time_per_pixel=0;
		TIMER0_Init(arr_x_time_per_pixel[index_x_time_per_pixel]);
		delay_ms(200);
	}
	else if(!P23) //右
	{
		delay_ms(1500); //暂停看波形
	}
}

