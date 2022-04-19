/**
  ******************************************************************************
  * @file    app_main_interface.c
  * @author  Team 32
  * @version V1.0
  * @date    18-04-2022
  * @brief   app_main_interface files
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//#define Mainfacedebug

#ifdef Mainfacedebug
  #define printf_Mainfacedebug        printf
#else
  #define printf_Mainfacedebug(...)
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/

/* Variable definition -------------------------------------------------------*/
/* Defines a font structure variable to set font parameters */
FONT_T   tFont;
/* Defines an edit box structure variable to display content */
EDIT_T EditIP;
/* Defines a window structure variable to display a prompt box */
WIN_T    tWin;
/* Defines a button structure variable to display a button in a prompt box */
BUTTON_T ButtonRet;


uint8_t TEM_ERR;
float   TEM_JU;
uint8_t XL_ERR;
uint8_t XL_JU;

/*
*********************************************************************************************************
*   Function name  : DIS_Page
*  Function description: Display display page
*  parameter: mode:  0:pressure unit   1:Height unit
*  The return value: None
*********************************************************************************************************
*/
static void DIS_Page(uint8_t mode)
{
  /* Fill the base */
  LCD_FillRoundRect(0, 18,90,160, 5,CL_BLUE);
  
  /* Draw the cross */
  LCD307_FillRect(80,18,90,2,CL_BLACK);
  LCD307_FillRect(0,18+45,2,160,CL_BLACK);
  
  /* Setting font Parameters */
  {
    tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
    tFont.FrontColor = CL_WHITE;  /* font color  */
    tFont.BackColor  = CL_MASK;   /* Text background color */
    tFont.Space      = 0;         /* Text spacing, in pixels*/
  }
  
  LCD_DispStrEx_NOmut(80-2-(6*3), 18+2,   "℃",&tFont,0,0);//009.2  099.2  100.2
  LCD_DispStrEx_NOmut(160-2-(6*3),18+2,   "bpm",&tFont,0,0);
  
  LCD_DispStrEx_NOmut(80-2-(6*3), 18+45+2,"lat",&tFont,0,0);
  LCD_DispStrEx_NOmut(160-2-(6*3),18+45+2,"lon",&tFont,0,0);
  
  DispLabelRound(0, 110,16,160,CL_BLUE,"               ",&tFont);
}

/*
*********************************************************************************************************
*   Function name  : Updata_Pre
*  Function description: Update pressure
*  parameter: P1 P2 P3 P4 : Pressure value or height value
*  The return value: None
*********************************************************************************************************
*/
static void Updata_Pre(float P1,float P2,float P3,float P4)
{
  char buf[80];
  
  /* Setting font Parameters */
  {
    tFont.FontCode   = FC_ST_16;  /* Font code 16 dots */
    tFont.FrontColor = CL_WHITE;  /* font color  */
    tFont.BackColor  = CL_BLUE;   /* Text background color */
    tFont.Space      = 0;         /* Text spacing, in pixels*/
  }
  
  if(P1 == 0)
  {
    LCD_DispStrEx_NOmut(0+05,23+15,"------",&tFont,48,ALIGN_CENTER);
  }
  else
  {
    sprintf((char *)&buf[0], "%6.2f", P1);
    LCD_DispStrEx_NOmut(0+05,23+15,buf,&tFont,48,ALIGN_CENTER);
  }
  
  if(P2 == 0)
  {
    LCD_DispStrEx_NOmut(80+05,23+15,"------",&tFont,48,ALIGN_CENTER);
  }
  else
  {
    sprintf((char *)&buf[0], "%6.0f", P2);
    LCD_DispStrEx_NOmut(80+05,23+15,buf,&tFont,48,ALIGN_CENTER);
  }
  
  if(P3 == 0)
  {
    LCD_DispStrEx_NOmut(0+05,23+45+15,"---------",&tFont,54,ALIGN_CENTER);
  }
  else
  {
    sprintf((char *)&buf[0], "%9.5f", P3);
    LCD_DispStrEx_NOmut(0+05,23+45+15,buf,&tFont,54,ALIGN_CENTER);
  }
  
  if(P4 == 0)
  {
    LCD_DispStrEx_NOmut(80+05,23+45+15,"---------",&tFont,54,ALIGN_CENTER);
  }
  else
  {
    sprintf((char *)&buf[0], "%9.5f", P4);
    LCD_DispStrEx_NOmut(80+05,23+45+15,buf,&tFont,54,ALIGN_CENTER);
  }
}

/*
*********************************************************************************************************
*   Function name  : MainInterface
*  Function description: Display main interface
*  parameter: None
*  The return value: status word
*********************************************************************************************************
*/
uint8_t   xl_count;    /* Heart rate measurement count */
uint8_t MainInterface(void)
{
  OS_RESULT xResult;     /* Email get results */
  uint8_t   ucKeyCode;   /* The key code */
  uint8_t   display;     /* Count display */
  uint8_t   xl_flag;     /* Heart rate measurement marker */
  uint8_t   xl_err;      /* Abnormal heart rate alarm */
  
  /* Clear Screen */
  LCD_ClrScr(RGB(0,0,0));
  
  /* Setting font Parameters */
  {
    tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
    tFont.FrontColor = CL_WHITE;  /* font color  */
    tFont.BackColor  = CL_BLUE;   /* Text background color */
    tFont.Space      = 1;         /* Text spacing, in pixels*/
  }
  LCD_DispStrEx_NOmut(20,2,"智能轮椅控制系统", &tFont,120,ALIGN_CENTER);
  
  /* Shows the base map and crosses and units */
  DIS_Page(0);
  
  /* Update the value */
  Updata_Pre(0,0,0,0);
  
  /* Open the backlight */
  LCD_SetPwmBackLight(g_tParam.ucBackLight);
  
  /* reset count display */
  display = 0;
  xl_flag = 0;
  xl_err = 0;
  
  HEART_ON();
  
  srand(ADCConvertedValue[0]);
  xl_count = 65 + rand()%20;
  
  while(1)
  {
    /* wait for 100ms */
    os_dly_wait(100);
    
    /* refresh */
    display ++;
    if(display >= 10)
    {
      display = 0;
      
      if((xl_flag % 2) == 0)
      {
        HEART_ON();
        xl_flag ++;
        
        if(TEM_ERR == 1)
        {
          /* buzzer warning */
          BEEP_Start(10,5,3);
          
          /* Setting font Parameters */
          {
            tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
            tFont.FrontColor = CL_WHITE;  /* font color  */
            tFont.BackColor  = CL_MASK;   /* Text background color */
            tFont.Space      = 0;         /* Text spacing, in pixels*/
          }
          DispLabelRound(0, 110,16,160,CL_BLUE,"温度高于阈值报警",&tFont);
        }
        else
        {
          /* Setting font Parameters */
          {
            tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
            tFont.FrontColor = CL_WHITE;  /* font color  */
            tFont.BackColor  = CL_MASK;   /* Text background color */
            tFont.Space      = 0;         /* Text spacing, in pixels*/
          }
          DispLabelRound(0, 110,16,160,CL_BLUE,"               ",&tFont);
        }
      }
      else
      {
        xl_flag = 0;
        HEART_OFF();
        
        if(XL_ERR == 1)
        {
          /* buzzer warning */
          BEEP_Start(10,5,3);
          
          /* Setting font Parameters */
          {
            tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
            tFont.FrontColor = CL_WHITE;  /* font color  */
            tFont.BackColor  = CL_MASK;   /* Text background color */
            tFont.Space      = 0;         /* Text spacing, in pixels*/
          }
          DispLabelRound(0, 110,16,160,CL_BLUE,"心率高于阈值报警",&tFont);
        }
        else
        {
          /* Setting font Parameters */
          {
            tFont.FontCode   = FC_ST_12;  /* Font code 12 dots */
            tFont.FrontColor = CL_WHITE;  /* font color  */
            tFont.BackColor  = CL_MASK;   /* Text background color */
            tFont.Space      = 0;         /* Text spacing, in pixels*/
          }
          DispLabelRound(0, 110,16,160,CL_BLUE,"               ",&tFont);
        }
      }
      
      if(xl_err == 0)
      {
        xl_count = 65 + rand()%20;
      }
      else
      {
        xl_err --;
      }
      /* Update the value */
      Updata_Pre(HUM_TEM_SHT20.Temperature,xl_count,LAT,LON);
      
      /* GPS positioning */
      if(gpsx.fix == 'A')
      {
        /* GPS positioning mark   */
        LCD_Fill_Rect(0,4,8,8,CL_GREEN);
      }
      else
      {
        /* GPS unlocated identifier   */
        LCD_Fill_Rect(0,4,8,8,CL_RED);
      }
      
      /* GSM module network check */
      if((GPIOC->IDR & GPIO_Pin_12) == 0)
      {
        /* GSM module networking identifier */
        LCD_Fill_Rect(153,4,8,8,CL_GREEN);
      }
      else
      {
        /* GSM module networking identifier */
        LCD_Fill_Rect(153,4,8,8,CL_RED);
      }
    }
    
    ucKeyCode = bsp_GetKey();  /* Handling key events */
    if (ucKeyCode != KEY_NONE)
    {
      switch (ucKeyCode)
      {
        case KEY_DOWN_K1:        /* K1 Key press */
          printf_Mainfacedebug("K1键按下, LED1点亮\r\n");
          break;
        
        case KEY_UP_K1:          /* K1 Key pop-up */
          printf_Mainfacedebug("K1键弹起, LED1熄灭\r\n");
          break;
        
        case KEY_LONG_K1:        /* K1 Key hold down */
          printf_Mainfacedebug("K1键长按\r\n");
          
          /* Enter the Setting screen */
          return MS_MACHINE_SET;
          break;
        
        case KEY_DOWN_K2:        /* K2 Key press */
          printf_Mainfacedebug("K2键按下, LED2点亮\r\n");
          break;
        
        case KEY_UP_K2:          /* K2 Key pop-up */
          printf_Mainfacedebug("K2键弹起, LED2熄灭\r\n");
          break;
        
        case KEY_LONG_K2:        /* K2 Key hold down */
          printf_Mainfacedebug("K2键长按\r\n");
          break;
        
        case KEY_DOWN_K3:        /* K3 Key press */
          printf_Mainfacedebug("K3键按下, LED3点亮\r\n");
          break;
        
        case KEY_UP_K3:          /* K3 Key pop-up */
          printf_Mainfacedebug("K3键弹起, LED3熄灭\r\n");
          break;
        
        case KEY_LONG_K3:        /* K3 Key hold down */
          printf_Mainfacedebug("K3键长按\r\n");
          
          /* Simulated abnormal heart rate */
          xl_count = g_tParam.Heart_rate_threshold + rand()%10;
          xl_err = 6;
          break;
        
        
        default:
          /* Other key values are not processed */
          break;
      }
    }
    
    /* The temperature exceeds the threshold */
    if(HUM_TEM_SHT20.Temperature >= g_tParam.Temperature_threshold)
    {
      TEM_ERR = 1;
      TEM_JU = HUM_TEM_SHT20.Temperature;
      bsp_LedOn(LED_D1);//The temperature alarm light is on
    }
    else
    {
      TEM_ERR = 0;
      bsp_LedOff(LED_D1);//The temperature alarm light is off
    }
    
    /* Heart rate over threshold alarm */
    if(xl_count >= g_tParam.Heart_rate_threshold)
    {
      XL_ERR = 1;
      XL_JU = xl_count;
      bsp_LedOn(LED_D2);//The heart rate alarm light is on
    }
    else
    {
      XL_ERR = 0;
      bsp_LedOff(LED_D2);//Heart rate alarm light off
    }
  }
}

