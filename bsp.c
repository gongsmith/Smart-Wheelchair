/**
  ******************************************************************************
  * @file    bsp.c
  * @author  Team 32
  * @version V1.0
  * @date    18-4-2022
  * @brief   bsp files
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//#define Bspdebug

#ifdef Bspdebug
  #define printf_Bspdebug        printf
#else
  #define printf_Bspdebug(...)
#endif

/* Private macro -------------------------------------------------------------*/
/* Define the routine name and routine release date */
#define EXAMPLE_NAME  "Car Suspension Controller---Remote Control"
#define EXAMPLE_DATE  "February 22, 2022"
#define DEMO_VER      "V1.0"

/* Private variables ---------------------------------------------------------*/
/* 任务堆栈 */
static uint64_t AppTaskStatStk[2048/8];              /* Statistics task stack */
static uint64_t AppTaskStartStk[2048/8];             /* start task stack */
static uint64_t AppTaskKeyStk[2048/8];               /* Process the task stack such as key scan */
static uint64_t AppTaskAdcStk[2048/8];               /* Read ADC task stack */
static uint64_t AppTaskDisplayStk[2048/8];           /* LCD display task stack */
static uint64_t AppTaskAppprocessStk[2048/8];        /* application process task stack */
static uint64_t AppTaskUsartStk[2048/8];             /* Serial communication task stack */
static uint64_t AppTaskWirelessStk[2048/8];          /* Wireless communication task stack */
static uint64_t AppTaskBlueStk[2048/8];              /* Bluetooth communication task stack */
static uint64_t AppTaskNetworkDataStk[2048/8];       /* Data processing task stack */

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/
extern __task void AppTask_Statistic(void);
extern __task void AppTask_Start(void);
extern __task void AppTask_Key(void);
extern __task void AppTask_Adc(void);
extern __task void AppTask_Display(void);
extern __task void AppTask_Appprocess(void);
extern __task void AppTask_Usart(void);
extern __task void AppTask_Wireless(void);
extern __task void AppTask_Blue(void);
extern __task void AppTask_NetworkData(void);


/* Variable definition -------------------------------------------------------*/
/* operating system boot logo */
volatile uint8_t OSRUN_Flag;

/* interface state */
volatile uint8_t ucStatus;

/* Statistics CPU usage task usage */
volatile uint32_t  OSIdleCtr;     /* idle task count */
volatile float     OSCPUUsage;    /* CPU percentage */
volatile uint32_t  OSIdleCtrMax;  /* Maximum idle count in 1 second */

/* task handle */
OS_TID HandleTaskStat          = NULL;
OS_TID HandleTaskStart         = NULL;
OS_TID HandleTaskKey           = NULL;
OS_TID HandleTaskAdc           = NULL;
OS_TID HandleTaskDisplay       = NULL;
OS_TID HandleTaskAppprocess    = NULL;
OS_TID HandleTaskUsart         = NULL;
OS_TID HandleTaskWireless      = NULL;
OS_TID HandleTaskBlue          = NULL;
OS_TID HandleTaskNetworkData   = NULL;

/* semaphore */
OS_SEM tcpc1semaphore;  /* define a semaphore */

/* Mutex semaphore */
OS_MUT spimutex;        /* Define an SPI bus mutex semaphore */

/* memory pool */
                         /* Declare a network memory pool，  10 block，36 bytes per block size */
_declare_box(netpool,    netPoolPerBlockSize,    netPoolBlocks);


/* message mailbox */
os_mbx_declare (netmailbox,     10);  /* Declare a web message mailbox that supports 10 messages */

/* Serial port receive start flag */
volatile uint8_t USART1RUN_Flag;
volatile uint8_t USART4RUN_Flag;
volatile uint8_t USART6RUN_Flag;

/*
*********************************************************************************************************
*  Function name: bsp_Init
*  Function description: Initialize all hardware devices. This function configures CPU registers and peripheral registers and initializes some global variables. only needs to be called once
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
uint8_t NrfExist;
void bsp_Init(void)
{
  /*
    Since the startup file of the ST firmware library has already performed the initialization of the CPU system clock, it is not necessary to configure the system clock again.
     The startup file configures the CPU main clock frequency, internal Flash access speed and optional external SRAM FSMC initialization.
      The default configuration of the system clock is 168MHz. If you need to change it, you can modify the system_stm32f4xx.c file
  */
  SystemCoreClockUpdate();
  
  /* NVIC configuration group 2 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* operating system boot logo */
  OSRUN_Flag = 0;
  
  bsp_InitLed();          /* Initial LED indicator port */
  bsp_InitKey();         /* Key ring area initialization */
  bsp_InitUart();        /* Initialize serial port */
  bsp_InitADC();         /* ADC initialization */
  BEEP_InitHard();       /* Buzzer initialization */
  bsp_InitI2C();         /* IIC initialization */
  bsp_InitIwdg(0x0FFF);  /* Initialize independent watchdog */
  
  /* Load parameters */
  LoadParam();
  
  /* Wait for the power supply to stabilize */
  bsp_DelayMS1(500);
  
  /* Check if SHT20 is normal */
  while(SHT20_CheckOk() == 0)
  {
    bsp_DelayMS1(100);
  }
  /* SHT20 initialization */
  SHT20_Init();
  
  /* LCD initialization */
  LCD_InitHard();
  
  /* 默认数据清零 */
  mem_set((char *)&g_tRun,0,sizeof(RUN_T));
  
  
  bsp_InitI2C_MAX();    /* initialization IIC */
  
  /* test if max30102 is normal */
  while(max30102_CheckOk() == 0);
  
  maxim_max30102_init();  /* initialization max30102 */
  HEART_OFF(); //close measure
}

/*
*********************************************************************************************************
*  Function name: StatisticsInit
*  Function description: Statistics task initialization
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void StatisticsInit (void)
{
  U32 cnt0;
  
  os_dly_wait(100);
  cnt0 = OSIdleCtr;
  os_dly_wait(100);
  cnt0 = OSIdleCtr - cnt0;
  OSIdleCtrMax = cnt0;
}

/*
*********************************************************************************************************
*  Function name: AppObjCreate
*  Function description: Create task communication mechanism
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void AppObjCreate (void)
{
  /* Create a semaphore with a count value of 0 for task synchronization */
  os_sem_init (&tcpc1semaphore, 0);
  
  /* Create mutex semaphore */
  os_mut_init (&spimutex);
  
  /* Initialize network memory pool，        4-byte alignment, 10 blocks, each block size is 52 bytes */
  _init_box (netpool,     sizeof (netpool),     netPoolPerBlockSize);
  
  /* Create an online message mailbox */
  os_mbx_init (&netmailbox,     sizeof(netmailbox));
}

/*
*********************************************************************************************************
*  Function name: System_bootCreate
*  Function description: Create system startup task
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void System_bootCreate (void)
{
  /* Create a startup task, create other tasks after idle */
  os_sys_init_user (AppTask_Start,             /* task function */
                    TaskStart_priority,        /* task priority */
                    &AppTaskStartStk,          /* task stack */
                    sizeof(AppTaskStartStk));  /* Task stack size, in bytes */
}

/*
*********************************************************************************************************
*  Function name: Statistical_taskCreate
*  Function description: Create statistical tasks
*  Formal parameters: none
*  Return value: none
*********************************************************************************************************
*/
void Statistical_taskCreate (void)
{
  HandleTaskStat = os_tsk_create_user(AppTask_Start,             /* task function */
                                      TaskStart_priority,        /* task priority */
                                      &AppTaskStartStk,          /* task stack */
                                      sizeof(AppTaskStartStk));  /* Task stack size, in bytes */
}

/*
*********************************************************************************************************
*  Function name: AppTaskCreate
*  Function description: Create application tasks
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
  /* Handle tasks such as key scanning */
  HandleTaskKey = os_tsk_create_user(AppTask_Key,           /* task function */ 
                                     TaskKey_priority,      /* task priority */ 
                                     &AppTaskKeyStk,        /* task stack */
                                     sizeof(AppTaskKeyStk));/* Task stack size, in bytes */
  
  /* Read ADC task */
  HandleTaskAdc = os_tsk_create_user(AppTask_Adc,           /* task function */ 
                                     TaskAdc_priority,      /* task priority */ 
                                     &AppTaskAdcStk,        /* task stack */
                                     sizeof(AppTaskAdcStk));/* Task stack size, in bytes */
  
  /* LCD display tasks*/
  HandleTaskDisplay = os_tsk_create_user(AppTask_Display,       /* task function */ 
                                     TaskDisplay_priority,      /* task priority */ 
                                     &AppTaskDisplayStk,        /* task stack */
                                     sizeof(AppTaskDisplayStk));/* Task stack size, in bytes */
  
  /* Application process task */
  HandleTaskAppprocess = os_tsk_create_user(AppTask_Appprocess,    /* task function */ 
                                     TaskAppprocess_priority,      /* task priority */ 
                                     &AppTaskAppprocessStk,        /* task stack */
                                     sizeof(AppTaskAppprocessStk));/* Task stack size, in bytes */
  
  /* Serial communication tasks */
  HandleTaskUsart = os_tsk_create_user(AppTask_Usart,         /* task function */ 
                                     TaskUsart_priority,      /* task priority */ 
                                     &AppTaskUsartStk,        /* task stack */
                                     sizeof(AppTaskUsartStk));/* Task stack size, in bytes */
  
  /* wireless communication tasks */
  HandleTaskWireless = os_tsk_create_user(AppTask_Wireless,      /* task function */ 
                                     TaskWireless_priority,      /* task priority */ 
                                     &AppTaskWirelessStk,        /* task stack */
                                     sizeof(AppTaskWirelessStk));/* Task stack size, in bytes */
  
  /* wireless communication tasks */
  HandleTaskBlue = os_tsk_create_user(AppTask_Blue,          /* task function */ 
                                     TaskBlue_priority,      /* task priority */ 
                                     &AppTaskBlueStk,        /* task stack */
                                     sizeof(AppTaskBlueStk));/* Task stack size, in bytes */
  
  /* data processing tasks */
  HandleTaskNetworkData = os_tsk_create_user(AppTask_NetworkData,   /* task function */ 
                                     TaskNetworkData_priority,      /* task priority */ 
                                     &AppTaskNetworkDataStk,        /* task stack */
                                     sizeof(AppTaskNetworkDataStk));/* Task stack size, in bytes */
}

/*
*********************************************************************************************************
*  Function name: bsp_RunPer10ms
*  Function description: This function is called once every 10ms by Systick interrupt. See the timer interrupt service routine in bsp_timer.c for details. Some require periodic processing
*  Transactions can be placed in this function. For example: key scanning, buzzer beeping control, etc.
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
  
}

/*
*********************************************************************************************************
*  Function name: bsp_RunPer1ms
*  Function description: This function is called once every 1ms by Systick interrupt. See the timer interrupt service routine in bsp_timer.c for details. some need to be processed periodically
*  Transactions can be placed in this function. For example: touch coordinate scanning.
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
  
}

/*
*********************************************************************************************************
*  Function name: bsp_Idle
*  Function description: The function to be executed when idle. Generally, the main program needs to insert the CPU_IDLE() macro in the body of the for and while loops to call this function.
*  This function defaults to a no-op. Users can add the functions of feeding the dog and setting the CPU to enter sleep mode.
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void bsp_Idle(void)
{
  /* --- feed dog */

  /* --- Put the CPU into sleep, wake up by the Systick timing interrupt or other interrupts */

  /* For emWin graphics library, you can insert the polling functions required by the graphics library */
  //GUI_Exec();

  /* For uIP protocol implementation, uip polling function can be inserted */
}

void bsp_DelayMS1(uint16_t dly)
{
  uint32_t DELAY;
  
  DELAY = dly*10000;
  while(DELAY --);
  
  DELAY = dly*10000;
  while(DELAY --);
  
  DELAY = dly*10000;
  while(DELAY --);
  
  DELAY = dly*10000;
  while(DELAY --);
}

/*
*********************************************************************************************************
*  Function name: PrintfLogo
*  Function description: Print the name of the routine and the release date of the routine. After connecting the serial cable, open the HyperTerminal software of the PC to observe the results
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void PrintfLogo(void)
{
  /* test CPU ID */
  {
    /* Reference book：
      32.6.1 MCU device ID code
      33.1 Unique device ID register (96 bits)
    */
    uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

    CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
    CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
    CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

    printf("\r\nCPU : STM32F407ZGT6, LQFP144, UID = %08X %08X %08X\n\r"
      , CPU_Sn2, CPU_Sn1, CPU_Sn0);
  }
  
  printf("\r\n");
  printf("*************************************************************\r\n");
  printf("* routine name   : %s\r\n", EXAMPLE_NAME);  /* routine name */
  printf("* routine version   : %s\r\n", DEMO_VER);    /* print routine version */
  printf("* public date   : %s\r\n", EXAMPLE_DATE);  /* print public date */

  /* Print the ST firmware library version, these 3 definition macros are in the stm32f10x.h file */
  printf("* Firmware library version : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
      __STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
  printf("* \r\n");  /* print a line of spaces */
  printf("* QQ    : 1172240606 \r\n");
  printf("* Tel   : 15815599164 \r\n");
  printf("*************************************************************\n\r");
}

/************************ (C) COPYRIGHT 2018 COUNS Team *****END OF FILE****/
