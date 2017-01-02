#ifndef __WIFI_CONFIG_H
#define	__WIFI_CONFIG_H


#include "stm32f10x.h"


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


//* 数据类型 */
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;


#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF[ RX_BUF_MAX_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strPc_Fram_Record, strEsp8266_Fram_Record;


//* 函数 */
#define WiFi_RST_INIT                  GPIO_Config                //WiFi使用的ES引脚初始化函数，推挽输出 
#define WiFi_USART1_INIT               USART1_Config              //WiFi使用的串口1初始化函数，波特率9600
#define WiFi_USART2_INIT               USART2_Config              //WiFi使用的串口2初始化函数，波特率9600 
#define WiFi_NVIC_INIT                 NVIC_Configuration         //NVIC中断配置

void WiFi_Config( void );
void NVIC_Configuration( void );
void wifi_Config(void);
char* zhsj(uint32_t shuju);
#endif    /* __WIFI_CONFIG_H */
