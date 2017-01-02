/*
 *  模拟 SPI 应用 BSP
 */

#include "bsp_gpio_spi.h"


/*-------------------------------------------------------------------------------------------------------
 * XPT 2046 IRQ 引脚配置
 * 当IRQ  换成其他的引脚时，需要修改下面的两个函数
 * 同时stm32f10x_it.c中的中断服务函数也应修改
 */
static void TP_INT_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

    /*使能EXTI9_5 中断 */		      
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}
static void TP_INT_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC */
	TP_INT_NVIC_Config();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = TP_INT_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // 上拉输入
  GPIO_Init(TP_INT_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource9);  // PF9 
  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}
/*------------------------------------------------------------------------------------------------------
 * 模拟SPI的GPIO配置 当SPI的4根信号线换成其他的IO时候，只需要修改该函数头文件的宏定义即可
 */
void GPIO_SPI_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* 开启GPIO时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

  /* 模拟SPI GPIO初始化 */          
  GPIO_InitStructure.GPIO_Pin=SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* 拉低片选，选择XPT2046 */
  GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN);
  //GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);
  
  /* XPT2046 中断IO配置 */
  TP_INT_GPIO_Config();
}

