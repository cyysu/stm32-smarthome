/**
  ******************************************************************************
  * @file    bsp_gpio.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   gpio应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_wifigpio.h"   

 /**
  * @brief  初始化GPIO
  * @param  无
  * @retval 无
  */
void GPIO_Config( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

	
		/*开启GPIOA的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 

    /* 配置WiFi模块的片选（CH）引脚	复位重启（RST）引脚*/
		/*选择要控制的GPIOA0（CH）引脚和GPIOA1（RST）引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOA*/
		GPIO_Init( GPIOA, &GPIO_InitStructure );	 
	

		/* 拉低WiFi模块的片选引脚	*/
		GPIO_ResetBits( GPIOA, GPIO_Pin_0 );		
		
		/* 拉高WiFi模块的复位重启引脚	*/
		GPIO_SetBits( GPIOA, GPIO_Pin_1 );
		
 
}
/*********************************************END OF FILE**********************/
