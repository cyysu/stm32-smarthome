#ifndef __BSP_GPIO_SPI_H
#define	__BSP_GPIO_SPI_H

#include "stm32f10x.h"

#define	 SPI_CLK_PIN	  GPIO_Pin_7
#define  SPI_CLK_PORT	  GPIOG

#define	 SPI_MISO_PIN	  GPIO_Pin_6
#define	 SPI_MISO_PORT	GPIOF

#define	 SPI_MOSI_PIN	  GPIO_Pin_11
#define	 SPI_MOSI_PORT	GPIOF

#define  SPI_CS_PIN		  GPIO_Pin_10
#define  SPI_CS_PORT		GPIOF

#define  TP_INT_PIN	    GPIO_Pin_9
#define  TP_INT_PORT	  GPIOF
#define INT_IN_2046   GPIO_ReadInputDataBit(TP_INT_PORT,TP_INT_PIN)

// ´¥ÃþÆÁ¼ò³Æ TP
#define TP_DCLK(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CLK_PORT,SPI_CLK_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CLK_PORT,SPI_CLK_PIN)
#define TP_CS(a)	\
						if (a)	\
						GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);	\
						else		\
						GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN)
#define TP_DIN(a)	\
						if (a)	\
						GPIO_SetBits(SPI_MISO_PORT,SPI_MOSI_PIN);	\
						else		\
						GPIO_ResetBits(SPI_MISO_PORT,SPI_MOSI_PIN)

#define TP_DOUT		 GPIO_ReadInputDataBit(SPI_MISO_PORT,SPI_MISO_PIN)


            
void GPIO_SPI_Config(void);

#endif /* __BSP_GPIO_SPI_H */
