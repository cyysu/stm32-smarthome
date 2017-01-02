#include "bsp_rtc.h"
#include "bsp_ili9341_lcd.h"
/* 秒中断标志，进入秒中断时置1，当时间被刷新之后清0 */
__IO uint32_t TimeDisplay = 0;
__IO uint32_t TimeDisPlay=0;
/*星期，生肖用文字ASCII码*/
uint8_t const *WEEK_STR[] = {"日", "一", "二", "三", "四", "五", "六"};
uint8_t const *zodiac_sign[] = {"猪", "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗"};


/*
 * 函数名：NVIC_Configuration
 * 描述  ：配置RTC秒中断的主中断优先级为1，次优先级为0
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * 函数名：RTC_CheckAndConfig
 * 描述  ：检查并配置RTC
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_CheckAndConfig(struct rtc_time *tm)
{
   	  /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,
	  则需重新配置时间并询问用户调整时间*/
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
	//	printf("\r\n\r\n RTC not yet configured....");

		/* RTC Configuration */
		RTC_Configuration();
		
	//	printf("\r\n\r\n RTC configured....");

		/* Adjust time by users typed on the hyperterminal */
		Time_Adjust(tm);

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
	  /*启动无需设置新时钟*/
		/*检查是否掉电重启*/
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
	//	    printf("\r\n\r\n Power On Reset occurred....");
		}
		/*检查是否Reset复位*/
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
	//		printf("\r\n\r\n External Reset occurred....");
		}
	
//		printf("\r\n No need to configure RTC....");
		
		/*等待寄存器同步*/
		RTC_WaitForSynchro();
		
		/*允许RTC秒中断*/
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
		/*等待上次RTC寄存器写操作完成*/
		RTC_WaitForLastTask();
	}
	   /*定义了时钟输出宏，则配置校正时钟输出到PC13*/
	#ifdef RTCClockOutput_Enable
	  /* Enable PWR and BKP clocks */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	  /* Allow access to BKP Domain */
	  PWR_BackupAccessCmd(ENABLE);
	
	  /* Disable the Tamper Pin */
	  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                 functionality must be disabled */
	
	  /* Enable RTC Clock Output on Tamper Pin */
	  BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif
	
	  /* Clear reset flags */
	  RCC_ClearFlag();

}




/*
 * 函数名：RTC_Configuration
 * 描述  ：配置RTC
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	/* Reset Backup Domain */
	BKP_DeInit();
	
	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}
	
	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization 
	 * 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
	 */
	RTC_WaitForSynchro();
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Second */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*****
* 函数名  : USART_Scanf
* 描述    : 从微机超级终端获取数字值(把ASCII码转换为数字)
			本函数专用于RTC获取时间，若进行其它输入应用，要修改一下
* 输入    : - value 用户在超级终端中输入的数值
* 输出    : 输入字符的ASCII码对应的数值
* 调用    ：内部调用
***/

static uint8_t USART_Scanf(uint32_t value)
{
	  uint32_t index = 0;
	  uint32_t tmp[2] = {0, 0};
	
	  while (index < 2)
	  {
	    /* Loop until RXNE = 1 */
	    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	    {
			}
		
	    tmp[index++] = (USART_ReceiveData(USART1));
	    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))   /*数字0到9的ASCII码为0x30至0x39*/
	    { 		  
		      printf("\n\rPlease enter valid number between 0 and 9 -->:  ");
              index--; 		 
	    }
	  }
	  
	  /* 计算输入字符的ASCII码转换为数字*/
	  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
		
	  /* Checks */
	  if (index > value)
	  {
	    printf("\n\rPlease enter valid number between 0 and %d", value);
	    return 0xFF;
	  }
	  return index;
}

/*
 * 函数名：Time_Regulate
 * 描述  ：返回用户在超级终端中输入的时间值，并将值储存在
 *         RTC 计数寄存器中。
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：用户在超级终端中输入的时间值，单位为 s
 * 调用  ：内部调用
 */
void Time_Regulate(struct rtc_time *tm)
{
	  u32 Tmp_YY = 0xFF, Tmp_MM = 0xFF, Tmp_DD = 0xFF, Tmp_HH = 0xFF, Tmp_MI = 0xFF, Tmp_SS = 0xFF;
	
	  printf("\r\n=========================Time Settings==================");
	
	  printf("\r\n  请输入年份(Please Set Years):  20");

	  while (Tmp_YY == 0xFF)
	  {
	    Tmp_YY = USART_Scanf(99);
	  }

	  printf("\n\r  年份被设置为:  20%0.2d\n\r", Tmp_YY);

	  tm->tm_year = Tmp_YY+2000;
	
	  Tmp_MM = 0xFF;

	  printf("\r\n  请输入月份(Please Set Months):  ");

	  while (Tmp_MM == 0xFF)
	  {
	    Tmp_MM = USART_Scanf(12);
	  }

	  printf("\n\r  月份被设置为:  %d\n\r", Tmp_MM);

	  tm->tm_mon= Tmp_MM;
	
	  Tmp_DD = 0xFF;

	  printf("\r\n  请输入日期(Please Set Dates):  ");

	  while (Tmp_DD == 0xFF)
	  {
	    Tmp_DD = USART_Scanf(31);
	  }

	  printf("\n\r  日期被设置为:  %d\n\r", Tmp_DD);

	  tm->tm_mday= Tmp_DD;
	
	  Tmp_HH  = 0xFF;

	  printf("\r\n  请输入时钟(Please Set Hours):  ");

	  while (Tmp_HH == 0xFF)
	  {
	    Tmp_HH = USART_Scanf(23);
	  }

	  printf("\n\r  时钟被设置为:  %d\n\r", Tmp_HH );

	  tm->tm_hour= Tmp_HH;
	    
	  Tmp_MI = 0xFF;

	  printf("\r\n  请输入分钟(Please Set Minutes):  ");

	  while (Tmp_MI == 0xFF)
	  {
	    Tmp_MI = USART_Scanf(59);
	  }

	  printf("\n\r  分钟被设置为:  %d\n\r", Tmp_MI);

	  tm->tm_min= Tmp_MI;
	  
	  Tmp_SS = 0xFF;

	  printf("\r\n  请输入秒钟(Please Set Seconds):  ");

	  while (Tmp_SS == 0xFF)
	  {
	    Tmp_SS = USART_Scanf(59);
	  }

	  printf("\n\r  秒钟被设置为:  %d\n\r", Tmp_SS);

	  tm->tm_sec= Tmp_SS;
}

/*
 * 函数名：Time_Show
 * 描述  ：在超级终端中显示当前时间值
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */ 
void Time_Show(struct rtc_time *tm)
{	 
	  /* Infinite loop */
//		while(1)
	    /* 每过1s */
//		{
	    if (TimeDisplay!=0)
	    {
				/* Display current time */
	      Time_Display( RTC_GetCounter(),tm); 		  
	      TimeDisplay = 0;
				TimeDisPlay++;
				if(TimeDisPlay==10)
					TimeDisPlay=0;
					
	    }
//	  }
}


/*
 * 函数名：Time_Adjust
 * 描述  ：时间调节
 * 输入  ：用于读取RTC时间的结构体指针
 * 输出  ：无
 * 调用  ：外部调用
 */
void Time_Adjust(struct rtc_time *tm)
{
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
	
	  /* Get time entred by the user on the hyperterminal */
	  Time_Regulate(tm);
	  
	  /* Get wday */
	  GregorianDay(tm);

	  /* 修改当前RTC计数寄存器内容 */
	  RTC_SetCounter(mktimev(tm));

	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
}

/*
 * 函数名：Time_Display
 * 描述  ：显示当前时间值
 * 输入  ：-TimeVar RTC计数值，单位为 s
 * 输出  ：无
 * 调用  ：内部调用
 */	
void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
//	   static uint32_t FirstDisplay = 1;
	   uint32_t BJ_TimeVar;
//	   uint8_t str[15]; // 字符串暂存  	

	   /*  把标准时间转换为北京时间*/
	   BJ_TimeVar =TimeVar + 8*60*60;

	   to_tm(BJ_TimeVar, tm);/*把定时器的值转换为北京时间*/	
	
//	  if((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay))
//	  {
//	      
//	      GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);	
//					printf("\r\n 今天新历：%0.2d%0.2d,%0.2d,%0.2d", str[0], str[1], str[2],  str[3]);
//	      GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
//					printf("\r\n 今天农历：%s\r\n", str);
//	
//	     if(GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
//					printf("\r\n 今天农历：%s\r\n", str);
//	
//	      FirstDisplay = 0;
//	  }	 	  	

	  /* 输出时间戳，公历时间 */
//	  printf(" UNIX时间戳 = %d 当前时间为: %d年(%s年) %d月 %d日 (星期%s)  %0.2d:%0.2d:%0.2d\r",TimeVar,
//	                    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday, 
//	                    WEEK_STR[tm->tm_wday], tm->tm_hour, 
//	                    tm->tm_min, tm->tm_sec);
		
		LCD_DisNum(10, 105,  tm->tm_year, WHITE);
		LCD_DisNum(45, 105,  tm->tm_mon, WHITE);
		LCD_DisNum(60, 105,  tm->tm_mday, WHITE);
		LCD_DispStrCH(80, 100, (uint8_t *)zodiac_sign[(tm->tm_year-3)%12],WHITE);
		LCD_DispStrCH(10, 132, (uint8_t *)"星期",WHITE);
		LCD_DispStrCH(42, 132, (uint8_t *)WEEK_STR[tm->tm_wday],WHITE);	
		LCD_DisNum(10, 150,  tm->tm_hour, WHITE);
		LCD_DispStr(22, 150, (uint8_t *)":",WHITE);
		LCD_DisNum(34, 150,  tm->tm_min, WHITE);
		LCD_DispStr(46, 150, (uint8_t *)":",WHITE);
		LCD_DisNum(52, 150,  tm->tm_sec, WHITE);
}




/************************END OF FILE***************************************/
