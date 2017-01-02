#ifndef __WIFI_FUNCTION_H
#define	__WIFI_FUNCTION_H 


#include "stm32f10x.h"
#include "wifi_config.h"
#include <stdbool.h>


#define     ESP8266_Usart( fmt, ... )           USART2_printf ( USART2, fmt, ##__VA_ARGS__ ) 
#define     PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )

#define     ESP8266_CH_HIGH_LEVEL()             GPIO_SetBits( GPIOA, GPIO_Pin_0 )
#define     ESP8266_CH_LOW_LEVEL()              GPIO_ResetBits( GPIOA, GPIO_Pin_0 )

#define     ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits( GPIOA, GPIO_Pin_1 )
#define     ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits( GPIOA, GPIO_Pin_1 )


void        ESP8266_Choose                      ( FunctionalState enumChoose );
void        ESP8266_Rst                         ( void );
void        ESP8266_AT_Test                     ( void );
bool        ESP8266_Cmd                         ( char * cmd, char * reply1, char * reply2, u32 waittime );
bool        ESP8266_Net_Mode_Choose             ( ENUM_Net_ModeTypeDef enumMode );
bool        ESP8266_JoinAP                      ( char * pSSID, char * pPassWord );
bool        ESP8266_BuildAP                     ( char * pSSID, char * pPassWord, char * enunPsdMode );
bool        ESP8266_Enable_MultipleId           ( FunctionalState enumEnUnvarnishTx );
bool        ESP8266_Link_Server                 ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool        ESP8266_StartOrShutServer           ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
bool        ESP8266_SendString                  ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
bool        ESP8266_Sendshu                     ( uint32_t pStr);
char *      ESP8266_ReceiveString               ( FunctionalState enumEnUnvarnishTx );

void        ESP8266_STA_TCP_Client              ( void );
void        ESP8266_AP_TCP_Server               ( void );
void        ESP8266_StaTcpClient_ApTcpServer    ( void );


#endif    /* __WIFI_FUNCTION_H */





