#ifndef __BSP_TOUCH_H
#define	__BSP_TOUCH_H

#include "stm32f10x.h"

/* 液晶坐标结构体 */
typedef	struct POINT 
{
   uint16_t x;		
   uint16_t y;
}Coordinate;

/*  校正系数结构体 */
typedef struct Parameter 
{						
long double An,  			 //注:sizeof(long double) = 8
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
}Parameter ;

extern volatile unsigned char touch_flag;
extern uint8_t cal_flag;
extern long double cal_p[6];

extern Coordinate ScreenSample[4];
extern Coordinate DisplaySample[4];
extern Parameter touch_para ;
extern Coordinate  display ;

#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字


void Touch_Init(void);
int Touch_Calibrate(void);
FunctionalState Get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para );
Coordinate *Read_2046_2(void);
uint16_t toch_sure(void);
uint16_t toch_1(void);
uint16_t toch_2(void);
uint16_t toch_return(void);
uint16_t toch_update(void);
uint16_t   toch(void);
void Touch_1(void);
void Touch_2(void);
uint32_t oneMonthData(uint32_t *a);

#endif /* __BSP_TOUCH_H */

