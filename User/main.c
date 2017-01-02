#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_dht11.h"
#include "bsp_usart1.h"
#include "bsp_adc.h"
#include "bsp_rtc.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_spi_flash.h"
#include "bsp_touch.h"
#include "bsp_TiMbase.h" 
#include "bsp_wifigpio.h"
#include "bsp_usart2.h"
#include "wifi_config.h"
#include "wifi_function.h"

struct rtc_time systmtime;
DHT11_Data_TypeDef DHT11_Data;
extern __IO uint16_t ADC_ConvertedValue[2];
float ADC_ConvertedValueLocal;
uint32_t smokescope;
uint32_t pm2_5;
volatile u32 time = 0; // ms 计时变量 
volatile unsigned char touch_flag=0;
unsigned char   TouchFlag_return_1,i;
extern __IO uint32_t TimeDisPlay;;
uint32_t temperatureArray[140];
uint32_t humidityArray[140];
uint32_t smokeArray[140];
uint32_t PM2_5Array[140];
uint32_t monthtemp[30];
uint32_t monthhum[30];
uint32_t monthsmoke[30];
uint32_t monthPM2_5[30];
void LCD_Show();
int aa=0;
void wifi_send();
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{

	int count = 0;
	int t = 0;
	int j =0;
	SysTick_Init();  
	LCD_Init();
  Lcd_GramScan(2);
	LCD_Clear(0, 0, 320, 240, BACKGROUND);//清屏
	USART1_Config();
	USART2_Config();
	SPI_FLASH_Init();
	Touch_Init();
	ADC1_Init();
	PM2_5_GPIO_INIT();
  DHT11_GPIO_Config();	/*初始化DTT11的引脚*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 , ENABLE);
	RTC_NVIC_Config();
	wifi_Config();
	/*-------------------------------------------------------------------------------------------------------*/  

  #if 1
  SPI_FLASH_BufferRead(&cal_flag, 0, 1);
  if( cal_flag == 0x55 )
  {
    SPI_FLASH_BufferRead((void*)cal_p, 1, sizeof(cal_p));
    SPI_FLASH_CS_HIGH();
  }
  else
  {
    /* 等待触摸屏校正完毕 */
    while(Touch_Calibrate() !=0);
  }
  #elif 0
  /* 等待触摸屏校正完毕 */
  while(Touch_Calibrate() !=0);
  #endif
/*------------------------------------------------------------------------------------------------------*/ 


  while(1)
  {
		/*
			时钟显示
		*/
		RTC_CheckAndConfig(&systmtime);
    Time_Show(&systmtime);
	  if (TimeDisPlay==0)
		{
				count++;
			  Read_DHT11(&DHT11_Data);
				pm2_5=PM2_5();
				pm2_5=(pm2_5*0.17-0.1);
				ADC_ConvertedValueLocal =(float) ADC_ConvertedValue[1]/4096*5;
				smokescope=ADC_ConvertedValueLocal*100;
    
				if(count == 1)
				{
					temperatureArray[t] = DHT11_Data.temp_int;
					humidityArray[t] = DHT11_Data.humi_int;
					smokeArray[t] = smokescope;
					PM2_5Array[t] = pm2_5;
				
					if(t == 139)
					{
						monthtemp[j] = oneMonthData(temperatureArray);
						monthhum[j] = oneMonthData(humidityArray);
						monthsmoke[j] = oneMonthData(smokeArray);
						monthPM2_5[j] = oneMonthData(PM2_5Array);
						t = 0;
						j++;
						if(j >= 30)
						{
							j = 0;
						}	
					}
						t++;
					count = 0;
				}
			
		}
	  /*
		 *lcd显示
		*/
		LCD_Show();
		while(touch_flag==1)
		{
				TouchFlag_return_1=toch();
			  if(TouchFlag_return_1==1)
				{
					LCD_Clear(0, 0, 320, 240, BACKGROUND);
					touch_flag=0;
				}	
						if(aa++>500)
		{wifi_send();
			aa=0;
		}		
    } 
		/*0000000000
		 *串口发送
		*/
//		printf("烟雾浓度为%f",ADC_ConvertedValueLocal);
//		printf("PM2.5为%f",pm2_5);
			/*调用Read_DHT11读取温湿度，若成功则输出该信息*/
//		if( Read_DHT11(&DHT11_Data)==SUCCESS)
//		{
////			printf("湿度为%d.%x ％RH ，温度为 %d.%x℃ \r\n",\
//			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);

//		}
//		else
//		{
////			printf("Read DHT11 ERROR!\r\n");
//		}

//	  /* Display time in infinite loop */																																																																																																																																
}
}

void LCD_Show()
{
		LCD_DispStrCH(144, 210, (uint8_t *)"刷新",  RED);	
		LCD_DispStrCH(0, 210, (uint8_t *)"确定",  RED);	
		LCD_DispStrCH(10, 10, (uint8_t *)"湿度",  RED);
		LCD_DispStrCH(10, 30, (uint8_t *)"温度",  RED);
		LCD_DispStrCH(10, 50, (uint8_t *)"烟雾浓度",RED);
		LCD_DispEnCh(10, 70, "PM2.5", RED);
	  LCD_DisNum(100, 10, DHT11_Data.humi_int, WHITE);
		LCD_DisNum(100, 30, DHT11_Data.temp_int, WHITE);
		LCD_DisNum(100, 50, 0, WHITE);
		LCD_DispStr(106, 50, (uint8_t *)".",WHITE);
		LCD_DisNum(112, 50, smokescope, WHITE);
		LCD_DisNum(100, 70, pm2_5, WHITE);	
}
void wifi_send()
{
	USART2_printf( USART2, " 湿度；\r\n" );
	ESP8266_Sendshu(DHT11_Data.humi_int);
	USART2_printf( USART2, " 温度；\r\n" );
	ESP8266_Sendshu(DHT11_Data.temp_int);
	USART2_printf( USART2, " 烟雾浓度；\r\n" );
	ESP8266_Sendshu(smokescope);
	USART2_printf( USART2, " PM2.5；\r\n" );
	ESP8266_Sendshu(pm2_5);
}
/*********************************************END OF FILE**********************/
