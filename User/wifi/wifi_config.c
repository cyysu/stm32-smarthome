/**
  ******************************************************************************
  * @file    wifi_config.c
  * @author  fire
  * @version V1.0
  * @date    2014-xx-xx
  * @brief   WiFi模块接口配置驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-STM32 开发板
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
	*/

#include "wifi_config.h"
#include "bsp_wifigpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "wifi_function.h"


struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };


/**
  * @brief  WiFi_Config wifi 初始化
  * @param  无
  * @retval 无
  */
void WiFi_Config( void )
{
	WiFi_RST_INIT();
  WiFi_USART1_INIT(); 
	WiFi_USART2_INIT(); 
	WiFi_NVIC_INIT();
  
}


/// 配置NVIC中的UART2中断
void NVIC_Configuration( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
void wifi_Config(void)
{	
	ESP8266_Net_Mode_Choose(STA_AP);//STA模
	//ESP8266_Rst();
	ESP8266_Enable_MultipleId(DISABLE);//单链接
	ESP8266_UnvarnishSend ( );//开启透传模式
	
	
}
char* zhsj(uint32_t shuju)
{
	char str[5];
	str[0]=shuju/100+48;
	str[1]=shuju%100/10+48;
	str[2]=shuju%10+48;
	return str;
	
}

/*********************************************************end of file**************************************************/
