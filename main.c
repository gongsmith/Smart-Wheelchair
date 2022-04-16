/**
  ******************************************************************************
  * @file    main.c
  * @author  COUNS Team
  * @version V1.0
  * @date    22-Jan-2019
  * @brief   main files
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2018 COUNS</center></h2>
  *
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/
//#define maindebug
 
#ifdef maindebug
  #define printf_maindbg        printf
#else
  #define printf_maindbg(...)
#endif

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/

/* Variable definition -------------------------------------------------------*/
uint8_t revbuff[64];
float   TEMP;

//temperature threshold
//heart rate threshold
//set speed threshold
//Display temperature, heart rate, longitude and latitude
//The normal heart rate is 60 to 100 beats per minute, which is the heart rate of sinus node pacing

//#define MAX_FLITER 2
//int32_t sum1=0,sum2=0;

/*
*********************************************************************************************************
*  Function name: main
*  Function description: c program entry
*  Formal parameters: none
*  Return value: error code (no processing required)
*********************************************************************************************************
*/
int main(void)
{
  uint8_t i;
  
  /* Initialize peripherals */
  bsp_Init();
  /* Print logo */
  //PrintfLogo();
  
  /* Create system, start RTX */
  System_bootCreate();
  
  while(1);
}

/*
*********************************************************************************************************
*  Function name: AppTask_Statistic
*  Function description: Statistics task, used to realize the statistics of CPU utilization. In order to test more accurately, you can turn on the global interrupt switch of annotation call
*  Formal parameters: none
*  return value: none
*  Priority: 1
*********************************************************************************************************
*/
__task void  AppTask_Statistic (void)
{
  U32 last_cnt = 0;
  U32 last_tick;
  U32 cnt, ticks;
  
  while(1)
  {
    /* The first time this function is called */
    if (last_cnt == 0)
    {
      last_cnt = OSIdleCtr | 1;
      last_tick = os_time;
    }
    else
    {
      /* Calculate the relative increment */
      cnt = OSIdleCtr - last_cnt;
      ticks = os_time - last_tick;
      /* Update the benchmark, next time use */
      last_cnt = OSIdleCtr | 1;
      last_tick = os_time;
      /* This judgment seems impossible */
      if (ticks != 0)
      {
        /* Calculate usage */
        cnt = cnt * 100 / ticks;
        if(cnt > OSIdleCtrMax)
        {
          OSCPUUsage = (cnt - OSIdleCtrMax) * 100.0 / OSIdleCtrMax;
        }
        else
        {
          OSCPUUsage = (OSIdleCtrMax - cnt) * 100.0 / OSIdleCtrMax;
        }
      }
    }
    
    /* Delay 1000ms to measure CPU usage once */
    os_dly_wait(1000);
    
    /* Running light is flashing */
    bsp_LedToggle(LED);
    
    /* feed dog */
    IWDG_Feed();
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Start
*  Function description: start task
*  Formal parameters: none
*  return value: none
*  Priority: 2
*********************************************************************************************************
*/
__task void AppTask_Start(void)
{
  /* Statistics task initialization */
  StatisticsInit();
  
  /* Prioritize the creation of statistical tasks */
  Statistical_taskCreate();
  
  /* Get the start task handle */
  HandleTaskStart = os_tsk_self();
  
  /* Create a task communication mechanism */
  AppObjCreate();
  
  /* Create a task */
  AppTaskCreate();
  
  /* operating system boot logo */
  OSRUN_Flag = 1;
  
  while(1)
  {
    os_dly_wait(100);
    
    if(ADCConvertedValue[0] < g_tParam.Speed_threshold)
    {
      if(ADCConvertedValue[0] < 800)
      {
        /* stop */
        TIM3->CCR3 = 0;
      }
      else
      {
        /* run */
        TIM3->CCR3 = ADCConvertedValue[0]/40;
      }
    }
    else
    {
      /* speeding */
      TIM3->CCR3 = 0;
    }
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Key
*  Function description: Handle tasks such as key scanning
*  Formal parameters: none
*  return value: none
*  Priority: 3
*********************************************************************************************************
*/
__task void  AppTask_Key (void)
{
  while(1)
  {
    bsp_KeyScan();
    BEEP_Pro();
    
    os_dly_wait(10);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Adc
*  Function description: read ADC task
*  Formal parameters: none
*  return value: none
*  Priority: 4
*********************************************************************************************************
*/
__task void  AppTask_Adc (void)
{
  char buf[256];
  U16  LEN;
  
//admin,smsend,16675529213,The elderly are in physical condition, please seek medical attention in time！89
//Heart rate: 78bpm89
//body temperature: 36.7℃89
//Location:114.06455184, 22.5484566489
  while(1)
  {
    if(TEM_ERR == 1 || XL_ERR == 1)
    {
      if((GPIOC->IDR & GPIO_Pin_12) == 0)
      {
        //send a messege
        
        sprintf((char *)&buf[0], "admin,smsend,17854559178,The elderly are in physical condition, please seek medical attention in time！\r\n");
        LEN = strlen(&buf[0]);
        comSendBuf(COM4,buf,LEN);
        
        os_dly_wait(2000);
        
        if(TEM_ERR == 1 && XL_ERR == 0)
        {
          sprintf((char *)&buf[0], "admin,smsend,17854559178,Heart rate:%3d [body temperature:%.2f] location:%9.5fE %9.5fN\r\n",xl_count,HUM_TEM_SHT20.Temperature,LON,LAT);
          LEN = strlen(&buf[0]);
          comSendBuf(COM4,buf,LEN);
        }
        else if(TEM_ERR == 0 && XL_ERR == 1)
        {
          sprintf((char *)&buf[0], "admin,smsend,17854559178,[Heart rate:%3d] body temperature:%.2f location:%9.5fE %9.5fN\r\n",xl_count,HUM_TEM_SHT20.Temperature,LON,LAT);
          LEN = strlen(&buf[0]);
          comSendBuf(COM4,buf,LEN);
        }
        else if(TEM_ERR == 1 && XL_ERR == 1)
        {
          sprintf((char *)&buf[0], "admin,smsend,17854559178,[Heart rate:%3d] [body temperature:%.2f] location:%9.5fE %9.5fN\r\n",xl_count,HUM_TEM_SHT20.Temperature,LON,LAT);
          LEN = strlen(&buf[0]);
          comSendBuf(COM4,buf,LEN);
        }
      }
      else
      {
        BEEP_Start(10,2,10);
      }
      
      os_dly_wait(60000);
    }
    os_dly_wait(100);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Display
*  Function description: LCD display task/process processing task
*  Formal parameters: none
*  return value: none
*  Priority: 5
*********************************************************************************************************
*/
__task void  AppTask_Display (void)
{
  uint8_t i;
  
  ucStatus = MS_MAIN_PAGE;  /* initial state = main interface */
  
  while(1)
  {
    switch (ucStatus)
    {
      case MS_MAIN_PAGE:
        ucStatus = MainInterface();/* main interface */
        break;
      
      case MS_MACHINE_SET:
        ucStatus = MACHINE_SET();/* Machine setting interface */
        break;
        
        case MACHINE_SET_1:
          MACHINE_SET_ONE();/* Backlight setting interface */
          break;
        
        case MACHINE_SET_2:
          MACHINE_SET_TWO();/* Temperature threshold setting interface */
          break;
        
        case MACHINE_SET_3:
          MACHINE_SET_THREE();/* Heart rate threshold setting interface */
          break;
        
        case MACHINE_SET_4:
          MACHINE_SET_FOUR();/* Speed threshold setting interface */
          break;
      
      default:
        ucStatus = MS_MAIN_PAGE;/* main interface */
        break;
    }
    
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Appprocess
*  Function description: application process task, execute door open button, remote door open, key value into buffer, WG input, door open process, WG send key value
*  Formal parameters: none
*  return value: none
*  Priority: 6
*********************************************************************************************************
*/
__task void  AppTask_Appprocess (void)
{
  uint8_t i;
  
  while(1)
  {
    if(SHT20_ReadTEMHUM(HUM_TEM_SHT20.Buf) == 1)
    {
      if(SHT20_CRC8(&HUM_TEM_SHT20.Buf[0],2) == HUM_TEM_SHT20.Buf[2] && SHT20_CRC8(&HUM_TEM_SHT20.Buf[3],2) == HUM_TEM_SHT20.Buf[5])
      {
        /*Convert actual temperature*/
        HUM_TEM_SHT20.Temperature = (175.72 * (float)((HUM_TEM_SHT20.Buf[0]<<8) | HUM_TEM_SHT20.Buf[1])/65535.0 - 46.85) ;
        HUM_TEM_SHT20.Humidity    = (125.0 * (float)((HUM_TEM_SHT20.Buf[3]<<8) | HUM_TEM_SHT20.Buf[4])/65535.0-6);
      }
      else
      {
        HUM_TEM_SHT20.Temperature = 0;
      }
    }
    else
    {
      HUM_TEM_SHT20.Temperature = 0;
    }
    
    os_dly_wait(800);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Usart
*  Function description: serial communication task, execute door open button, open door remotely, key value into buffer, WG input, open door process, WG send key value
*  Formal parameters: none
*  return value: none
*  Priority: 7
*********************************************************************************************************
*/
//Simulate GPS data
const uint8_t USART1_TX_BUF[] = \
"\
$GPRMC,143919.00,A,2238.67885,N,11402.85722,E,0.609,35.92,080821,,,A*78\r\n\
$GPVTG,,T,,M,0.609,N,1.128,K,A*26\r\n\
$GPGGA,143919.00,2238.67885,N,11402.85722,E,1,03,3.49,93.4,M,-2.5,M,,*71\r\n\
$GPGSA,A,3,26,32,10,,,,,,,,,,7.87,3.49,7.05*02\r\n\
$GPGSV,3,1,10,01,00,284,,03,08,321,,10,35,174,34,22,23,309,18*7E\r\n\
$GPGSV,3,2,10,23,06,159,,25,28,040,10,26,52,213,31,29,19,097,*77\r\n\
$GPGSV,3,3,10,31,52,326,,32,61,060,23*7B\r\n\
";

__task void  AppTask_Usart (void)
{
  OS_RESULT xResult;
  U16 i;
  U16 GPS_COUNT;
  
  GPS_COUNT = 0;
  
  while(1)
  {
    /* Judging whether the serial port reception times out --- Since the host computer has strict requirements on this time, here we use 10ms once */
    if((USART1RUN_Flag & 0x7F) > 95)
    {
      for(i=0;i<g_tUart1.usRxCount;i++)
      {
        GPS_BUF[i] = g_tUart1.pRxBuf[i];
      }
      
      //bsp_LedToggle(LED_D1);
      
      GPS_Analysis(&gpsx,(u8*)GPS_BUF);//Parse the string
      
      //GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//stimulate
      
      if(gpsx.fix == 'A')
      {
        LAT = gpsx.latitude/100000.0f;
        LON = gpsx.longitude/100000.0f;
      }
      else
      {
        LAT = 0;
        LON = 0;
      }
      
      USART1RUN_Flag = 0x00;
      comClearRxFifo(COM1);
      
      GPS_COUNT = 0;
    }
    
    /* Judging whether the serial port reception times out --- Since the host computer has strict requirements on this time, here we use 10ms once */
    if((USART4RUN_Flag & 0x7F) > 10)
    {
      USART4RUN_Flag = 0;
      comClearRxFifo(COM4);
    }
    
    /* GPS Timeout Count */
    GPS_COUNT ++;
    if(GPS_COUNT >= 300)
    {
      gpsx.fix = 0;
      GPS_COUNT = 0;
      LAT = 0;
      LON = 0;
    }
    os_dly_wait(10);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Wireless
*  Function description: wireless communication task, execute door opening button, remote door opening, key value into buffer, WG input, door opening process, WG send key value
*  Formal parameters: none
*  return value: none
*  Priority: 8
*********************************************************************************************************
*/
__task void  AppTask_Wireless (void)
{
  OS_RESULT xResult;
  
  while(1)
  {
    os_dly_wait(1000);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_Blue
*  Function description: Bluetooth communication task, execute door open button, open door remotely, key value into buffer, WG input, door open process, WG send key value
*  Formal parameters: none
*  return value: none
*  Priority: 9
*********************************************************************************************************
*/
__task void  AppTask_Blue (void)
{
  while(1)
  {
    os_dly_wait(2000);
  }
}

/*
*********************************************************************************************************
*  Function name: AppTask_NetworkData
*  Function description: data processing task, mainly processing all data
*  Formal parameters: none
*  return value: none
*  Priority: 10
*********************************************************************************************************
*/
__task void AppTask_NetworkData(void)
{
  while(1)
  {
    os_dly_wait(2000);
  }
}

/************************ (C) COPYRIGHT 2018 COUNS Team *****END OF FILE****/
