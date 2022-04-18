/**
  ******************************************************************************
  * @file    app_machine_set.c
  * @author  Team 32
  * @version V1.0
  * @date    18-04-2022
  * @brief   app_machine_set files
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "bsp.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//#define Machine_setdebug

#ifdef Machine_setdebug
  #define printf_Machine_setdebug        printf
#else
  #define printf_Machine_setdebug(...)
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* section text */
const char s_tMachine_settxt[][22] = 
{
  {"01.Backlight settings"},
  {"02.Temperature Threshold Settings"},
  {"03.Heart Rate Threshold Settings"},
  {"04.Speed Threshold Settings"},
};

/* After the icon is clicked, switch the program status */
static const uint8_t s_RetStatus[12] =
{
  MACHINE_SET_1,     /* Backlight settings */
  MACHINE_SET_2,     /* Temperature Threshold Settings */
  MACHINE_SET_3,     /* Heart Rate Threshold Settings */
  MACHINE_SET_4,     /* Speed Threshold Settings */
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/

/* Variable definition -------------------------------------------------------*/
/* The index of the selected column, 0 means no column is selected, the first column is selected by default */
int8_t s_ColumnCursor_machine_set = 1;
/* page code */
uint8_t PageCode_machine_set = 0;

/* Remote control data needs to be sent to the main board */
uint8_t Save_Flag = 0;

/*
*********************************************************************************************************
*  Function name: DispTopBar
*  Function description: Display the title bar
*  Formal parameter: const char * pbuf
*  return value: none
*********************************************************************************************************
*/
void DispTopBar(const char * pbuf)
{
  /* Set font parameters*/
  {
    tFont.FontCode   = FC_ST_16;  /* Font code 16 dot matrix */
    tFont.FrontColor = CL_WHITE;  /* Font color */
    tFont.BackColor  = CL_MASK;   /* Text background color */
    tFont.Space      = 1;         /* Text spacing, unit = pixels */
  }
  
  LCD_Fill_Rect(0, 0, TOP_BAR_H, g_LcdWidth, CL_BAR);
  LCD_DispStrEx(5, 2,pbuf, &tFont,150,ALIGN_CENTER);
}

/*
*********************************************************************************************************
*  Function name: DispBottomBar
*  Function description: Display the bottom bar
*  Formal parameter: const char * pbuf
*  return value: none
*********************************************************************************************************
*/
void DispBottomBar(const char * pbuf)
{
  /* Set font parameters */
  {
    tFont.FontCode   = FC_ST_16;  /* Font code 16 dot matrix */
    tFont.FrontColor = CL_WHITE;  /* font color */
    tFont.BackColor  = CL_BAR;    /* Text background color */
    tFont.Space      = 1;         /* Text spacing, unit = pixels */
  }
  LCD_Fill_Rect(0, g_LcdHeight - BOTTOM_BAR_H, BOTTOM_BAR_H, g_LcdWidth, CL_BAR);
  
  LCD_DispStrEx(5, g_LcdHeight - 18,pbuf, &tFont,150,ALIGN_CENTER);
}

/*
*********************************************************************************************************
*  Function name: DispBackground
*  Function description: Display interface background
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void DispBackground(void)
{
  /* fill background color */
  LCD_Fill_Rect(0, TOP_BAR_H, g_LcdHeight - TOP_BAR_H - BOTTOM_BAR_H, g_LcdWidth, RGB(0,78,152));
}

/*
*********************************************************************************************************
*  Function name: Array_Main_Column
*  Function description: Arrange the main column
*  Formal parameter: const char *_tbuf: points to the first address of the string
*  uint8_t stlen: Since the string is a two-dimensional variable-length array, parameters cannot be passed normally, and the length of the string is specified here
*  uint8_t _tlen: the number of column lines
*  return value: none
*********************************************************************************************************
*/
void Array_Main_Column(const char *_tbuf,uint8_t stlen,uint8_t _tlen)
{
  uint8_t i;
  uint8_t j;
  
  /* Set font parameters */
  {
    tFont.FontCode   = FC_ST_12;       /* Font code 12 dot matrix */
    tFont.FrontColor = CL_GREY2;       /* Font color */
    tFont.BackColor  = RGB(0,78,152);  /* Text background color */
    tFont.Space      = 1;              /* Text spacing, unit = pixels */
  }
  
  j = 0;
  for(i=0;i<_tlen;i++)
  {
    /* Show column text */                      /* Here 140 cannot be changed to 0 */
    LCD_DispStrEx(10, 22+j,&_tbuf[0]+stlen*i, &tFont,140,ALIGN_LEFT);
    /* Show the bottom line between columns */
    LCD_DrawLine(10,34+j, 149, 34+j,RGB(100,90,100));
    j += 14;
  }
}

/*
*********************************************************************************************************
*  Function name: SYS_INFO
*  Function description: Display the 【Machine Settings】interface
*  Formal parameters: none
*  return value: status word
*********************************************************************************************************
*/
uint8_t MACHINE_SET(void)
{
  uint8_t  ucKeyCode;  /* key code */
  uint8_t  fRefresh;   /* Refresh request flag, 1 means need to refresh */
  uint16_t timecount;  /* 30 seconds count */
  
  DispTopBar((const char *)"machine settings");  /* Show top bar */
  DispBackground();                                /* Show icon background */
  DispBottomBar((const char *)"*:quit  #:confirm");/* Show bottom bar */
  
  /* Arrange main columns */
  Array_Main_Column(&s_tMachine_settxt[0][0],22,4);
  
  /* Set font parameters */
  {
    tFont.FrontColor = CL_ORANGE1;       /* font color */
    tFont.BackColor  = RGB(100,90,100);  /* font background color */
  }
  
  /* Select the Nth column */
  LCD_DispStrEx(10, 22+(s_ColumnCursor_machine_set-1-PageCode_machine_set)*14,(char *)&s_tMachine_settxt[s_ColumnCursor_machine_set-1][0], &tFont,140,ALIGN_LEFT);
  
  /* Timeout count cleared */
  timecount = 0;
  
  /* 1 Indicates that the LCD needs to be refreshed */
  fRefresh = 0;
  
  /* Wait for the button to be released */
  while((GPIOC->IDR  & GPIO_Pin_2) == 0)
  {
    os_dly_wait(100);
  }
  bsp_ClearKey();
  
  while (1)
  {
    if (fRefresh)
    {
      fRefresh = 0;
      
      PageCode_machine_set = 0;
      /* 排列主栏目 */
      Array_Main_Column(&s_tMachine_settxt[0][0],22,4);
      
      /* set font parameters */
      {
        tFont.FrontColor = CL_ORANGE1;       /* font color */
        tFont.BackColor  = RGB(100,90,100);  /* font background color */
      }
      
      /* Select the Nth column */
      LCD_DispStrEx(10, 22+(s_ColumnCursor_machine_set-1-PageCode_machine_set)*14,(char *)&s_tMachine_settxt[s_ColumnCursor_machine_set-1][0], &tFont,140,ALIGN_LEFT);
    }
    
    /* Delay 100ms */
    os_dly_wait(100);
    
    /* 30S timeout exit mechanism */
    timecount ++;
    if(timecount == 300)
    {
      /* The next time you enter, the first column is selected by default */
      s_ColumnCursor_machine_set = 1;
      PageCode_machine_set = 0;
      
      /* back to main interface */
      return MS_MAIN_PAGE;
    }
    
    /* Handling key events */
    ucKeyCode = bsp_GetKey();
    if (ucKeyCode != KEY_NONE)
    {
      /* Timeout count cleared */
      timecount = 0;
      
      switch (ucKeyCode)
      {
        case KEY_3_DOWN:  /* 3 key press */
        case KEY_4_DOWN:  /* 4 key press */
          {
            /* set font parameters */
            {
              tFont.FrontColor = CL_GREY2;       /* font color */
              tFont.BackColor  = RGB(0,78,152);  /* font background color */
            }
            
            /* Uncheck the Nth column */
            LCD_DispStrEx(10, 22+(s_ColumnCursor_machine_set-1-PageCode_machine_set)*14,(char *)&s_tMachine_settxt[s_ColumnCursor_machine_set-1][0], &tFont,140,ALIGN_LEFT);
          }
          
          if(ucKeyCode == KEY_3_DOWN)
          {
            s_ColumnCursor_machine_set --;
            if(s_ColumnCursor_machine_set == 0)
              s_ColumnCursor_machine_set = 4;
          }
          else
          {
            s_ColumnCursor_machine_set ++;
            if(s_ColumnCursor_machine_set == 5)
              s_ColumnCursor_machine_set = 1;
          }
          fRefresh = 1;
          break;
          
        case KEY_1_DOWN:  /* * key press   */
          /* The next time you enter, the first column is selected by default */
          s_ColumnCursor_machine_set = 1;
          PageCode_machine_set = 0;
          /* back to main interface */
          return MS_MAIN_PAGE;
        
        case KEY_2_DOWN:  /* # key press */
          return s_RetStatus[s_ColumnCursor_machine_set-1];
          
          default:
            break;
      }
    }
  }
}
