/**
  ******************************************************************************
  * @file    app_tft_lcd.c
  * @author  Team 32
  * @version V1.0
  * @date    16-04-2022
  * @brief   app_tft_lcd files
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

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* 0,portrait mode；1,landscape mode；2,180°portrait mode；3,180°landscape mode*/
#define DISP_MODE   3    /* Display mode */

/* Private function prototypes -----------------------------------------------*/
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf);
static void _LCD_ReadAsciiDot_NOmut(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
static void _LCD_ReadHZDot_NOmut(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf);

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/

/* Variable definition -------------------------------------------------------*/
uint8_t  displaybuf[176];/* support 32*44 dot matrix Cinese Character */

/*
*********************************************************************************************************
*  Function name: LCD_HardReset
*  Function discription: Reset hardware. 
*  Parameters: void
*  Return: void
*********************************************************************************************************
*/
void LCD_HardReset(void)
{

}

/*
*********************************************************************************************************
*  Function name: LCD_PwmBackLight_Init
*  Function discription: initialize LCD back light controlling GPIO,configure as PWM mode
*  Parameters: void
*  Return: void
*********************************************************************************************************
*/
void LCD_PwmBackLight_Init(void)
{
  /* Back light output by CPU controlled by PWM,PC9/TIM3_CH4 */
  GPIO_InitTypeDef         GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef        TIM_OCInitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  
  /* Configure GPIO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; /* formal parameter */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* Connect to AF function */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM3);
  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100;
  TIM_TimeBaseStructure.TIM_Prescaler = 999;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  
  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  /* TIMx enable counter */
  TIM_Cmd(TIM3, ENABLE);
  
  /*  */
  TIM3->CCR3 = 0;
}

/*
*********************************************************************************************************
*  Function name: LCD_SetPwmBackLight
*  Function discription: set brightness of back light
*  Parameters: _bright brightness,0 is darkest,100 is brightest, this is a precentage parameter
*  Return: void
*********************************************************************************************************
*/
void LCD_SetPwmBackLight(uint8_t _bright)
{
  /* Back light output by CPU controlled by PWM,PC9 */
  TIM3->CCR4 = _bright;
}

/*
*********************************************************************************************************
*  Function name: LCD_InitHard
*  Function discription: Initialize LCD
*  Parameters: void
*  Return: void
*********************************************************************************************************
*/
void LCD_InitHard(void)
{
  LCD_PwmBackLight_Init(); /* Initialize back light control IO, back light off by defult. */
  LCD_HardReset();         /* Reset hardware */
  TFTLCD_Init();           /* Initialize LCD */
  
#ifdef USE_ILI9341
  LCD_Display_Dir(DISP_MODE);/* 0,portrait mode；1,landscape mode；2,portrait mode180°；3,landscape mode180°*/
  LCD_ClrScr(RGB(0,0,0)); /* Clear screen */
#elif defined USE_ST7789V
  LCD_Display_Dir(DISP_MODE);/* 0,portrait mode；1,landscape mode；2,portrait mode180°；3,landscape mode180°*/
  LCD_ClrScr(RGB(0,0,0)); /* Clear screen */
#endif
  LCD_SetPwmBackLight(0);    /* Disable back light */
}

/*
*********************************************************************************************************
*  Function name: LCD_GetHeight
*  Function discription: Read height of LCD resolution ratio.
*  Parameters: void
*  Return: LCD height
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
  return g_LcdHeight;
}

/*
*********************************************************************************************************
*  Function name: LCD_GetWidth
*  Function discription: Read width of LCD resolution ratio.
*  Parameters: void
*  Return: LCD width
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
  return g_LcdWidth;
}

/*
*********************************************************************************************************
*  Function name: LCD_DispOn
*  Function discription: turn on the display
*  Parameters: void
*  Return: void
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
  LCD_DisplayOn();
}

/*
*********************************************************************************************************
*  Function name: LCD_DispOff
*  Function discription: turn off the diaplay
*  Parameters: void
*  Return: void
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
  LCD_DisplayOff();
}

/*
*********************************************************************************************************
*  Function name: LCD_ClrScr
*  Function discription: Clear screen by the input color
*  Parameters: _usColor : Background color
*  Return: void
*********************************************************************************************************
*/
void LCD_ClrScr(uint16_t _usColor)
{
  LCD307_ClrScr(_usColor);
}

/*
*********************************************************************************************************
*  Function name: _LCD_ReadAsciiDot
*  Function discription: read dot matrix data of single ASCII character 
*  Parameters:
*    _code     : ASCII character code,1 byte, 1-128
*    _fontcode :Font code
*    _pBuf     : storage for dot matrix data
*  Return: Character width
*********************************************************************************************************
*/
//with mutex
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
  const uint8_t *pAscDot;
  uint8_t  font_bytes;
  
  pAscDot = 0;
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 12;
      pAscDot = g_Ascii12;
      break;
    
    case FC_ST_16:
      /* 16dot matrix by defult */
      font_bytes = 16;
      pAscDot = g_Ascii16;
      break;
    
    case FC_ST_24:
      font_bytes = 48;
      pAscDot = g_Ascii24;
      break;
    
    case FC_ST_32:
      font_bytes = 64;
      pAscDot = g_Ascii32;
      break;
      
    /* Display space */
    default:
      _code = 0x20;
      font_bytes = 64;
      pAscDot = g_Ascii32;
      break;
  }
  
  /* Copy ascii character dot matrix of Flash inside of CPU to buf */
  memcpy(_pBuf, &pAscDot[_code * font_bytes/*(font_bytes / 2)*/], font_bytes/*(font_bytes / 2)*/);
}

/*
*********************************************************************************************************
*  Function name: _LCD_ReadAsciiDot_NOmut
*  Function discription: read dot matrix data of single ASCII character 
*  Parameters:
*    _code     : ASCII character code,1 byte 1-128
*    _fontcode :Font code
*    _pBuf     : storage for dot matrix data
*  Return: Character width
*********************************************************************************************************
*/
///without mutex
static void _LCD_ReadAsciiDot_NOmut(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
  const uint8_t *pAscDot;
  uint8_t  font_bytes;
  
  pAscDot = 0;
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 12;
      pAscDot = g_Ascii12;
      break;
    
    case FC_ST_16:
      /* 16dot matrix by defult */
      font_bytes = 16;
      pAscDot = g_Ascii16;
      break;
    
    case FC_ST_24:
      font_bytes = 48;
      pAscDot = g_Ascii24;
      break;
    
    case FC_ST_32:
      font_bytes = 64;
      pAscDot = g_Ascii32;
      break;
      
    /* display space */
    default:
      _code = 0x20;
      font_bytes = 64;
      pAscDot = g_Ascii32;
      break;
  }
  
  /* Copy ascii character dot matrix of Flash inside of CPU to buf */
  memcpy(_pBuf, &pAscDot[_code * font_bytes/*(font_bytes / 2)*/], font_bytes/*(font_bytes / 2)*/);
}

/*
*********************************************************************************************************
*  Function name: _LCD_ReadHZDot
*  Function discription: read dot matrix data of single Chinese character
*  Parameters:
*    _code1, _cod2 : CICS code. GB2312 code
*    _fontcode     : Font code
*    _pBuf         : storage for dot matrix data
*  Return: void
*********************************************************************************************************
*/
//with mutex
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT  /* With Flash word stock inside of CPU*/
  
  uint8_t  *pDot;
  uint8_t  font_bytes;
  uint32_t address;
  uint16_t m;
  
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 24;
      pDot = (uint8_t *)g_Hz12;
      break;
      
    case FC_ST_16:    /* 16dot matrix */
      font_bytes = 32;
      pDot = (uint8_t *)g_Hz16;
      break;
      
    case FC_ST_24:    /* 24dot matrix */
      font_bytes = 72;
      pDot = (uint8_t *)g_Hz24;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_bytes = 128;
      pDot = (uint8_t *)g_Hz32;
      break;
    
    /* Display space */
    default:
      font_bytes = 128;
      pDot = (uint8_t *)g_Hz32;
      break;
  }
  
  m = 0;
  while(1)
  {
    address = m * (font_bytes + 2);
    m++;
    if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
    {
      address += 2;
      memcpy(_pBuf, &pDot[address], font_bytes);
      break;
    }
    else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
    {
      /* Not found,padding with FF */
      memset(_pBuf, 0xFF, font_bytes);
      break;
    }
  }
#else  /* Use the whole word stock */
  uint8_t  font_bytes;
  uint32_t addr;
  
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 24;
      addr = Font_hz12_addr;
      break;
    
    case FC_ST_16:    /* 16dot matrix */
      font_bytes = 32;
      addr = Font_hz16_addr;
      break;

    case FC_ST_24:    /* 24dot matrix */
      font_bytes = 72;
      addr = Font_hz24_addr;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_bytes = 128;
      break;
    
    /* Display space */
    default:
      font_bytes = 128;
      break;
  }
  
  /* 此处需要根据字库文件存放位置进行修改 */
/*
GBK2313    Address calculation formula
  if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
  {
    addr += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
  }
  else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
  {
    addr += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
  }
*/
  
//GBK  Address calculation formula
  addr += ((_code1 - 0x81) * 190 + (_code2 - 0x40) -(_code2 >> 7)) * font_bytes;
  
  /* Wait for SPI mutually exclusive resources */
  os_mut_wait (&spimutex, 0xFFFF);
  
  sf_ReadBuffer(_pBuf,addr,(font_bytes));
  
//  /* Read data with DMA */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(addr);
//  sf_FastReadBuffer_Duad_IO_Read_DMA(_pBuf,font_bytes);
//  sf_FastReadBuffer_Duad_IO_End_DMA();
  
  /* Release SPI mutually exclusive resources */
  os_mut_release (&spimutex);
  
#endif
}

/*
*********************************************************************************************************
*  Function name: _LCD_ReadHZDot_NOmut
*  Function discription: read dot matrix data of single Chinese character
*  Parameters:
*    _code1, _cod2 : CICS code. GB2312 Code 
*    _fontcode     : Font code
*    _pBuf         : storage for dot matrix data
*  Return: void
*********************************************************************************************************
*/
//without mutex
static void _LCD_ReadHZDot_NOmut(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT  /* With Flash word stock inside of CPU*/
  
  uint8_t  *pDot;
  uint8_t  font_bytes;
  uint32_t address;
  uint16_t m;
  
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 24;
      pDot = (uint8_t *)g_Hz12;
      break;
      
    case FC_ST_16:    /* 16dot matrix */
      font_bytes = 32;
      pDot = (uint8_t *)g_Hz16;
      break;
      
    case FC_ST_24:    /* 24dot matrix */
      font_bytes = 72;
      pDot = (uint8_t *)g_Hz24;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_bytes = 128;
      pDot = (uint8_t *)g_Hz32;
      break;
    
    /* Display space */
    default:
      font_bytes = 128;
      pDot = (uint8_t *)g_Hz32;
      break;
  }
  
  m = 0;
  while(1)
  {
    address = m * (font_bytes + 2);
    m++;
    if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
    {
      address += 2;
      memcpy(_pBuf, &pDot[address], font_bytes);
      break;
    }
    else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
    {
      /* Not found,padding with FF*/
      memset(_pBuf, 0xFF, font_bytes);
      break;
    }
  }
#else  /* Use full word stock */
  uint8_t  font_bytes;
  uint32_t addr;
  
  switch (_fontcode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_bytes = 24;
      addr = Font_hz12_addr;
      break;
    
    case FC_ST_16:    /* 16dot matrix */
      font_bytes = 32;
      addr = Font_hz16_addr;
      break;

    case FC_ST_24:    /* 24dot matrix */
      font_bytes = 72;
      addr = Font_hz24_addr;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_bytes = 128;
      break;
    
    /* Display space */
    default:
      font_bytes = 128;
      break;
  }
  
/*
GBK2313    Address calculation formula
  if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
  {
    addr += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
  }
  else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
  {
    addr += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
  }
*/
  
//GBK  Address calculation formula
  addr += ((_code1 - 0x81) * 190 + (_code2 - 0x40) -(_code2 >> 7)) * font_bytes;
  
  sf_ReadBuffer(_pBuf,addr,(font_bytes));
  
//  /* Read data with DMA */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(addr);
//  sf_FastReadBuffer_Duad_IO_Read_DMA(_pBuf,font_bytes);
//  sf_FastReadBuffer_Duad_IO_End_DMA();
  
#endif
}

/*
*********************************************************************************************************
*  Function name: LCD_GetFontWidth
*  Function discription: Read font width（pixel)
*  Parameters: _tFont: Font structure, including color, background color (support transparency), Font code, text spacing
*  Return: font width（pixel)
*********************************************************************************************************
*/
uint16_t LCD_GetFontWidth(FONT_T *_tFont)
{
  uint8_t font_width;
  
  switch (_tFont->FontCode)
  {
    case FC_ST_12:
      font_width = 12;
      break;

    case FC_ST_16:
      font_width = 16;
      break;
      
    case FC_ST_24:
      font_width = 24;
      break;
      
    case FC_ST_32:
      font_width = 32;
      break;
    
    default:
      font_width = 16;
    break;
  }
  return font_width;
}

/*
*********************************************************************************************************
*  Function name: LCD_GetFontHeight
*  Function discription: Read font height（pixel)
*  Parameters: _tFont: Font structure, including color, background color (support transparency), Font code, text spacing
*  Return: font height（pixel)
*********************************************************************************************************
*/
uint16_t LCD_GetFontHeight(FONT_T *_tFont)
{
  uint8_t height;
  
  switch (_tFont->FontCode)
  {
    case FC_ST_12:
      height = 12;
      break;

    case FC_ST_16:
      height = 16;
      break;
      
    case FC_ST_24:
      height = 24;
      break;
      
    case FC_ST_32:
      height = 32;
      break;
    
    default:
      height = 16;
    break;
  }
  return height;
}

/*
*********************************************************************************************************
*  Function name: LCD_GetStrWidth
*  Function discription: calculate the width of string(pixel)
*  Parameters:
*    _ptr  : string pointer
*    _tFont: Font structure, including color, background color (support transparency), Font code, text spacing
*    _ascii: Number of ASCII codes
*  Return: width of string
*********************************************************************************************************
*/
uint16_t LCD_GetStrWidth(const char *_ptr, FONT_T *_tFont,uint8_t *_ascii)
{
  const char     *p;
  uint16_t width;
  uint8_t  code1, code2;
  uint8_t  font_width;
  
  p     = _ptr;
  width = 0;
  
  font_width = LCD_GetFontWidth(_tFont);
  
  while (*p != 0)
  {
    code1 = *p;       /* Read string data, could be ASCII code or high byte of the Chinese character code */
    if (code1 < 0x80) /* ASCII */
    {
      switch(_tFont->FontCode)
      {
        case FC_ST_12:
          font_width = 6;
          break;

        case FC_ST_16:
          font_width = 8;
          break;
          
        case FC_ST_24:
          font_width = 12;
          break;
          
        case FC_ST_32:
          font_width = 16;
          break;
        
        default:
          font_width = 8;
          break;
      }
      (*_ascii) ++;
      
    }
    else/* Chinese Character */
    {
      code2 = *++p;
      if (code2 == 0)/* Check whether it is an invalid character */
      {
        break;
      }
      font_width = LCD_GetFontWidth(_tFont);
    }
    width += font_width;
    p++;
  }
  
  return width;
}

/*
*********************************************************************************************************
*  Function name: LCD_DispStrEx
*  Function discription: Display a string on the specified location of LCD（top left)
*  Parameters:
*    _usX   : X coordinates
*    _usY   : Y coordinates
*    _ptr   : string pointer
*    _tFont : Font structure, including color, background color (support transparency), Font code, text spacing, can display chinese character form word stock RA8875
*    _Width : Width for string display.
*    _Align :Alignment of strings in the display area,
*    ALIGN_LEFT   = 0,
*    ALIGN_CENTER = 1,
*    ALIGN_RIGHT  = 2
*  Return: void
*********************************************************************************************************
*/
void LCD_DispStrEx(uint16_t _usX, uint16_t _usY, const char *_ptr, FONT_T *_tFont, uint16_t _Width, uint8_t _Align)
{
  uint8_t  i;
  uint8_t  code1;
  uint8_t  code2;
  uint16_t str_width; /* String width  */
  uint16_t offset;
  uint8_t  width;
  uint8_t  m;
  uint16_t x,y;
  uint8_t  font_width;
  uint8_t  font_height;
  uint8_t  ascii;
  
  switch (_tFont->FontCode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_height = 12;
      font_width  = 12;
      break;
      
    case FC_ST_16:    /* 16dot matrix */
      font_height = 16;
      font_width  = 16;
      break;
      
    case FC_ST_24:    /* 24dot matrix */
      font_height = 24;
      font_width  = 24;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_height = 32;
      font_width  = 32;
      break;
    
    defualt:
      font_height = 16;
      font_width  = 16;
      break;
  }
  /* Number of ASCII codes */
  ascii  = 0;
  str_width = LCD_GetStrWidth(_ptr, _tFont, &ascii);  /* Calculates the actual width of the string */
  
  /* Calculates the actual width of the string(include space) */
  if(_tFont->Space != 0)
  {
    /* Maximum number of characters */
    offset = str_width / font_width * 2;
    
    /* Maximum number of spaces(In ASCII) */
    font_height = offset - 1;
    
    /* Calculate number of Chinese character*/
    offset = (offset - ascii) / 2;
    
    /* Calculate number of space */
    font_height = font_height - offset;
    
    /* Calculates the actual width of the string(include space) */
    str_width += font_height * _tFont->Space;
    
    /* Variable value recovery */
    font_height = font_width;
  }
  
  offset = 0;
  if (_Width > str_width)
  {
    if (_Align == ALIGN_RIGHT)        /* JustifyRight */
    {
      offset = _Width - str_width;
    }
    else if (_Align == ALIGN_CENTER)  /* JustifyCenter */
    {
      offset = (_Width - str_width) / 2;
    }
    else if (_Align == ALIGN_LEFT)    /* JustifyLeft */
    {
      /* Defult JustifyLeft */;
    }
  }
  
  if(_tFont->BackColor != CL_MASK)
  {
    /* Fill in the background color on the left, needed when JustifyCenter and JustifyRight  */
    if (offset > 0)
    {
      LCD_Fill_Rect(_usX, _usY, font_height/*LCD_GetFontHeight(_tFont)*/, offset, _tFont->BackColor);
      _usX += offset;
    }
    
    /* Fill in the background color on the Right,needed when JustifyCenter and JustifyLeft */
    if ((_Width > str_width) && (_Align != ALIGN_RIGHT))
    {
      LCD_Fill_Rect(_usX + str_width, _usY, font_height/*LCD_GetFontHeight(_tFont)*/, _Width - str_width - offset, _tFont->BackColor);
    }
  }
  else
  {
    if (offset > 0)
    {
      _usX += offset;
    }
  }
  
  /* Use CPU internal word stock .*/
  {
    /* Looping through characters */
    while (*_ptr != 0)
    {
      code1 = *_ptr;  /* Read string data, could be ASCII code or high byte of the Chinese character code */
      if (code1 < 0x80)
      {
        /* copy ascii character dot matrix to displaybuf */
        _LCD_ReadAsciiDot(code1-0x20, _tFont->FontCode, displaybuf);  /* Read ASCII character dot matrix */
        width = font_width / 2;
      }
      else
      {
        code2 = *++_ptr;
        if (code2 == 0)
        {
          break;
        }
        /* Copy dot matrix of one Chinese character to displaybuf */
        _LCD_ReadHZDot(code1, code2, _tFont->FontCode, displaybuf);
        width = font_width;
      }
      
      y = _usY;
      
      if(width <= 8)
        code1 = 1;
      else if(width <= 16)
        code1 = 2;
      else if(width <= 24)
        code1 = 3;
      else if(width <= 32)
        code1 = 4;
      
      /* LCD */
      for (m = 0; m < font_height; m++)  /* Character height */
      {
        x = _usX;
        code2 = m * code1;
        for (i = 0; i < width; i++)  /* Character width */
        {
          if((displaybuf[code2 + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
          {
            LCD_PutPixel(x, y, _tFont->FrontColor);  /* Set Pixel color to Character color */
          }
          else
          {
            if (_tFont->BackColor != CL_MASK)  /* Transparent */
            {
              LCD_PutPixel(x, y, _tFont->BackColor);  /* Set Pixel color to Background color */
            }
          }
          x++;
        }
        y++;
      }
      
      if ((_tFont->Space > 0) && (_tFont->BackColor != CL_MASK))
      {
        /* _tFont->usBackColor,word space is larger than width of dot matrix,Then you need to fill in between the words */
        LCD_Fill_Rect(_usX + width, _usY, font_height, _tFont->Space, _tFont->BackColor);
      }
      _usX += width + _tFont->Space;  /* Increase column address  */
      _ptr++;                         /* Move to the next character */
    }
  }
}

//without mutex
/*
*********************************************************************************************************
*  Function name: LCD_DispStrEx_NOmut
*  Function discription:  Display a string on the specified location of LCD（top left)
*  Parameters:
*    _usX   : X coordinates
*    _usY   : Y coordinates
*    _ptr   : string pointer
*    _tFont : Font structure, including color, background color (support transparency), Font code, text spacing
*    _Width : Width for string display.
*    _Align :Alignment of strings in the display area,
*    ALIGN_LEFT   = 0,
*    ALIGN_CENTER = 1,
*    ALIGN_RIGHT  = 2
*  Return: void
*********************************************************************************************************
*/
void LCD_DispStrEx_NOmut(uint16_t _usX, uint16_t _usY, const char *_ptr, FONT_T *_tFont, uint16_t _Width, uint8_t _Align)
{
  uint8_t  i;
  uint8_t  code1;
  uint8_t  code2;
  uint16_t str_width; /* String width  */
  uint16_t offset;
  uint8_t  width;
  uint8_t  m;
  uint16_t x,y;
  uint8_t  font_width;
  uint8_t  font_height;
  uint8_t  ascii;
  
  switch (_tFont->FontCode)
  {
    case FC_ST_12:    /* 12dot matrix */
      font_height = 12;
      font_width  = 12;
      break;
      
    case FC_ST_16:    /* 16dot matrix */
      font_height = 16;
      font_width  = 16;
      break;
      
    case FC_ST_24:    /* 24dot matrix */
      font_height = 24;
      font_width  = 24;
      break;
      
    case FC_ST_32:    /* 32dot matrix */
      font_height = 32;
      font_width  = 32;
      break;
    
    defualt:
      font_height = 16;
      font_width  = 16;
      break;
  }
  /* Number of ASCII codes */
  ascii  = 0;
  str_width = LCD_GetStrWidth(_ptr, _tFont, &ascii);  /* Calculates the actual width of the string */
  
  /* Calculates the actual width of the string(include space) */
  if(_tFont->Space != 0)
  {
    /* Maximum number of characters */
    offset = str_width / font_width * 2;
    
    /* Maximum number of spaces(In ASCII) */
    font_height = offset - 1;
    
    /* Calculate number of Chinese character*/
    offset = (offset - ascii) / 2;
    
    /* Calculate number of space */
    font_height = font_height - offset;
    
    /* Calculates the actual width of the string(include space) */
    str_width += font_height * _tFont->Space;
    
    /* Variable value recovery */
    font_height = font_width;
  }
  
  offset = 0;
  if (_Width > str_width)
  {
    if (_Align == ALIGN_RIGHT)        /* JustifyRight */
    {
      offset = _Width - str_width;
    }
    else if (_Align == ALIGN_CENTER)  /* JustifyCenter */
    {
      offset = (_Width - str_width) / 2;
    }
    else if (_Align == ALIGN_LEFT)    /* JustifyLeft */
    {
      /* JustifyLeft In defult*/;
    }
  }
  
  if(_tFont->BackColor != CL_MASK)
  {
    /* Fill in the background color on the left,when JustifyCenter and JustifyRight  */
    if (offset > 0)
    {
      LCD_Fill_Rect(_usX, _usY, font_height/*LCD_GetFontHeight(_tFont)*/, offset, _tFont->BackColor);
      _usX += offset;
    }
    
    /* Fill in the background color on the Right,when JustifyCenter and JustifyLeft */
    if ((_Width > str_width) && (_Align != ALIGN_RIGHT))
    {
      LCD_Fill_Rect(_usX + str_width, _usY, font_height/*LCD_GetFontHeight(_tFont)*/, _Width - str_width - offset, _tFont->BackColor);
    }
  }
  else
  {
    if (offset > 0)
    {
      _usX += offset;
    }
  }
  
  /* Use CPU internal word library */
  {
    /* Looping through characters */
    while (*_ptr != 0)
    {
      code1 = *_ptr;  /* Read string data, could be ASCII code or high byte of the Chinese character code */
      if (code1 < 0x80)
      {
        /* copy dot matrix of ascii character to displaybuf */
        _LCD_ReadAsciiDot_NOmut(code1-0x20, _tFont->FontCode, displaybuf);  /* Read dot matrix of ASCII Character */
        width = font_width / 2;
      }
      else
      {
        code2 = *++_ptr;
        if (code2 == 0)
        {
          break;
        }
        /* Copy dot matrix of one Chinese character to displaybuf */
        _LCD_ReadHZDot_NOmut(code1, code2, _tFont->FontCode, displaybuf);
        width = font_width;
      }
      
      y = _usY;
      
      if(width <= 8)
        code1 = 1;
      else if(width <= 16)
        code1 = 2;
      else if(width <= 24)
        code1 = 3;
      else if(width <= 32)
        code1 = 4;
      
      /* LCD */
      for (m = 0; m < font_height; m++)  /* Character height */
      {
        x = _usX;
        code2 = m * code1;
        for (i = 0; i < width; i++)  /* Character width */
        {
          if((displaybuf[code2 + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
          {
            LCD_PutPixel(x, y, _tFont->FrontColor);  /* Set Pixel color to Character color */
          }
          else
          {
            if (_tFont->BackColor != CL_MASK)  /* Transparent */
            {
              LCD_PutPixel(x, y, _tFont->BackColor);  /* Set Pixel color to Background color */
            }
          }
          x++;
        }
        y++;
      }
      
      if ((_tFont->Space > 0) && (_tFont->BackColor != CL_MASK))
      {
        /* _tFont->usBackColor,word space is larger than width of dot matrix,Then you need to fill in between the words */
        LCD_Fill_Rect(_usX + width, _usY, font_height, _tFont->Space, _tFont->BackColor);
      }
      _usX += width + _tFont->Space;  /* Increase column address  */
      _ptr++;                         /* Move to the next character */
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DispStr
*  Function discription: Displays a string at specified coordinate of LCD 
*  Parameters:
*     _usX : X coordinates
*     _usY : Y coordinates
*    _ptr  : string pointer
*    _tFont: Font structure, including color, background color (support transparency), Font code, text spacing
*  Return: void
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
  LCD_DispStrEx(_usX, _usY, _ptr, _tFont, 0, 0);
}

/*
*********************************************************************************************************
*  Function name: LCD_DispStr_12
*  Function discription: Displays a string at specified coordinate of LCD Can only display 6*12 ASCII,for word stock update
*  Parameters:
*    _usX  : X coordinates
*    _usY  : Y coordinates
*    _ptr  : string pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DispStr_12(uint16_t _usX, uint16_t _usY, char *_ptr)
{
  uint8_t  i;
  uint8_t  code1;
  uint8_t  m;
  uint16_t x,y;
  
  /* Use CPU internal word library */
  {
    /* Looping through characters */
    while (*_ptr != 0)
    {
      code1 = *_ptr;  /* Read string data, could be ASCII code or high byte of the Chinese character code */
      if (code1 < 0x80)
      {
        /* Copy dot matrix of ascii character to buf */
        memcpy(displaybuf, &g_Ascii12[(code1-0x20) * 12],12);
      }
      
      y = _usY;
      code1 = 1;
      
      /* LCD */
      for (m = 0; m < 12; m++)  /* Character height */
      {
        x = _usX;
        code1 = m;
        for (i = 0; i < 6; i++)  /* Character width */
        {
          if((displaybuf[code1 + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
          {
            LCD_PutPixel(x, y, CL_WHITE);  /* Set the pixel color to white */
          }
          x++;
        }
        y++;
      }
      _usX += 6;  /* Increase column address  */
      _ptr++;     /* Move to the next character */
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DispStr_64
*  Function discription: Displays a string at specified coordinate of LCD Can only display 32*64 ASCII,for word stock update
*  Parameters:
*    _usX  : X coordinates
*    _usY  : Y coordinates
*    _ptr  : string pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DispStr_64(uint16_t _usX, uint16_t _usY, char *_ptr)
{
  uint8_t  i;
  uint8_t  code1;
  uint8_t  m;
  uint16_t x,y;
  
  /* Use CPU internal word library */
  {
    /* Looping through characters */
    while (*_ptr != 0)
    {
      code1 = *_ptr;  /* Read string data, could be ASCII code or high byte of the Chinese character code */
      if (code1 < 0x80)
      {
        if(code1 == ':')
        {
          /* Copy dot matrix of ascii character to buf */
          memcpy(displaybuf, &g_Ascii64[1760],176);
        }
        else if(code1 == ' ')
        {
          memset((char *)displaybuf, 0x00,176);
          //memcpy(displaybuf, &g_Ascii64[1760],176);
        }
        else
        {
          code1 -= 0x30;
          
          /* Copy dot matrix of ascii character to buf */
          memcpy(displaybuf, &g_Ascii64[code1*176],176);
        }
      }
      
      y = _usY;
      code1 = 1;
      
      /* LCD */
      for (m = 0; m < 44; m++)  /* Character height */
      {
        x = _usX;
        code1 = m;
        for (i = 0; i < 32; i++)  /* Character width */
        {
          if((displaybuf[(code1<<2) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
          {
            LCD_PutPixel(x, y, CL_WHITE);  /* Set the pixel color to white */
          }
          x++;
        }
        y++;
      }
      _usX += 32;  /* Increase column address  */
      _ptr++;     /* Move to the next character */
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_PutPixel
*  Function discription: Draw 1 pixel
*  Parameters:
*      _usX,_usY : Pixel coordinates
*      _usColor  : Pixel color
*  Return: void
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
  LCD307_PutPixel(_usX, _usY, _usColor);
}

/*
*********************************************************************************************************
*  Function name: LCD_GetPixel
*  Function discription: Read 1 pixel
*  Parameters:
*      _usX,_usY : Pixel coordinates
*  Return: RGB value
*********************************************************************************************************
*/
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
  uint16_t usRGB;
  
  usRGB = LCD307_GetPixel(_usX, _usY);
  
  return usRGB;
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawLine
*  Function discription: Draw a line between two points with  Bresenham algorithm
*  Parameters:
*      _usX1, _usY1 : Starting point coordinates
*      _usX2, _usY2 : termination point coordinates
*      _usColor     : color
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{
  LCD307_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawPoints
*  Function discription: Draw a set of points and connect them with Bresenham algorithm for waveform display
*  Parameters:
*      x, y     : Coordinate array
*      _usColor : color
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
  uint16_t i;
  
  for (i = 0 ; i < _usSize - 1; i++)
  {
    LCD_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawRect
*  Function discription: Plot a horizontal placed  rectangle
*  Parameters:
*      _usX,_usY: Coordinates of the top left corner of the rectangle
*      _usHeight : Height of rectangle
*      _usWidth  : Width of rectangle
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
  LCD307_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
}

/*
*********************************************************************************************************
*  Function name: LCD_Fill_Rect
*  Function discription: Fill a rectangle with a color value,There is a LCD_FillRect in emWin, therefore add underline  to distinguish
*  Parameters:
*      _usX,_usY: Coordinates of the top left corner of the rectangle
*      _usHeight : Height of rectangle
*      _usWidth  : Width of rectangle
*  Return: void
************************************************************************************radius of a circle *********************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
  LCD307_FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawCircle
*  Function discription: Draw a circle with the 1 pixel pen width
*  Parameters:
*      _usX,_usY  : Coordinates of the center of the circle
*      _usRadius  : Radius of a circle 
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
  LCD307_DrawCircle(_usX, _usY, _usRadius, _usColor);
}

/*
*********************************************************************************************************
*  Function name: LCD_FullCircle
*  Function discription: Fill a circle
*  Parameters:
*      _usX,_usY  : Coordinates of the center of the circle
*      _usRadius  : Radius of a circle 
*  Return: void
*********************************************************************************************************
*/
void LCD_FullCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
  uint16_t x,y,r=_usRadius;
  
  for(y=_usY - r;y<_usY +r;y++)
  {
    for(x=_usX - r;x<_usX+r;x++)
    {
      if(((x-_usX)*(x-_usX)+(y-_usY)*(y-_usY)) <= r*r)
      {
        LCD_PutPixel(x,y,_usColor);
      }
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawBMP
*  Function discription: Display a BMP bidmap on LCD,bidmap dot matrix scanning ordering: left to right,from top to bottom
*  Parameters:
*      _usX, _usY : Coordinates of the picture
*      _usHeight  : Height of the picture
*      _usWidth   : Width of the picture
*      _ptr       : Pointer of picture dot matrix
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
  LCD307_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawTriangle_TOP
*  Function discription: Draw a triangle on the LCD
*  Parameters: _usX:X coordinate of vertex of a triangle
*            _usY:Y coordinate of vertex of a triangle
*       _usHeight:Height of triangle
*         _usmode:1:upright -1:invert
*        _usColor:color of triangle
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawTriangle_TOP(uint16_t _usX, uint16_t _usY, uint8_t _usHeight, int8_t _usmode, uint16_t _usColor)
{
  uint8_t i;
  
  for(i=0;i<_usHeight;i++)
  {
    LCD_DrawLine(_usX-i,_usY+i*_usmode,_usX+i,_usY+i*_usmode,_usColor);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawTriangle_BOTTOM
*  Function discription: Draw a triangle on the LCD
*  Parameters: _usX:X coordinates of base of a triangle
*            _usY:Y coordinates of base of a triangle
*        _usWidth:width of base of a triangle
*         _usmode:1:upright -1:invert
*        _usColor:color of triangle
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawTriangle_BOTTOM(uint16_t _usX, uint16_t _usY, uint8_t _usWidth, int8_t _usmode, uint16_t _usColor)
{
  uint8_t i;
  uint8_t j;
  
  j = _usWidth;
  
  if((j%2) == 1)
    return;
  
  for(i=0;;i++)
  {
    LCD_DrawLine(_usX+i,_usY-i*_usmode,_usX+i+j,_usY-i*_usmode,_usColor);
    j -= 2;
    if(j == 0)
    {
      i++;
      LCD_DrawLine(_usX+i,_usY-i*_usmode,_usX+i+j,_usY-i*_usmode,_usColor);
      return;
    }
  }
}

/****************************************************************************************/

/*
*********************************************************************************************************
*  Function name: LCD_DrawWin
*  Function discription: Draw a window on LCD
*  Parameters: _pWin:Window structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawWin(WIN_T *_pWin)
{
  //uint16_t TitleHegiht;
  //TitleHegiht = 20;
  
  #define TitleHegiht  20
  
  /* Draws the window outframe */
  LCD_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
  LCD_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);
  
  /* Window title bar */
  LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);
  
  /* Filling */
  LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht,
    _pWin->Width - 4, WIN_BODY_COLOR);
  
  LCD_DispStr(_pWin->Left + 6, _pWin->Top + 4, _pWin->pCaption, _pWin->Font);
  
  #undef TitleHegiht
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawIcon
*  Function discription: Draw an arc feet Icon on LCD
*  Parameters: _pIcon : Icon structure pointer
*            _tFont : Font properties
*      _ucFocusMode : FocusMode.0 Represent normal icon  1 Represents the selected icon
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
  const uint16_t *p;
        uint16_t usNewRGB;
        uint16_t x, y;      /* Record relative coordinates within a window */
  
  p = _tIcon->pBmp;
  for (y = 0; y < _tIcon->Height; y++)
  {
    for (x = 0; x < _tIcon->Width; x++)
    {
      usNewRGB = *p++;  /* After reading the color value of the icon, the pointer increases by 1 */
      /* The four right angles of the icon are cut into arc angles, and outside the arc Angle is the background icon */
      if ((y == 0 && (x < 6 || x > _tIcon->Width - 7)) ||
          (y == 1 && (x < 4 || x > _tIcon->Width - 5)) ||
          (y == 2 && (x < 3 || x > _tIcon->Width - 4)) ||
          (y == 3 && (x < 2 || x > _tIcon->Width - 3)) ||
          (y == 4 && (x < 1 || x > _tIcon->Width - 2)) ||
          (y == 5 && (x < 1 || x > _tIcon->Width - 2)) ||

          (y == _tIcon->Height - 1 && (x < 6 || x > _tIcon->Width - 7)) ||
          (y == _tIcon->Height - 2 && (x < 4 || x > _tIcon->Width - 5)) ||
          (y == _tIcon->Height - 3 && (x < 3 || x > _tIcon->Width - 4)) ||
          (y == _tIcon->Height - 4 && (x < 2 || x > _tIcon->Width - 3)) ||
          (y == _tIcon->Height - 5 && (x < 1 || x > _tIcon->Width - 2)) ||
          (y == _tIcon->Height - 6 && (x < 1 || x > _tIcon->Width - 2))
          )
      {
        ;
      }
      else
      {
        if (_ucFocusMode != 0)  /* 1Represents the selected icon */
        {
          /* Reduce the brightness of the original pixel to achieve the effect of the icon being activated */
          uint16_t R,G,B;
          uint16_t bright = 15;
          
          /* rrrr rggg gggb bbbb */
          R = (usNewRGB & 0xF800) >> 11;
          G = (usNewRGB & 0x07E0) >> 5;
          B =  usNewRGB & 0x001F;
          if (R > bright)
          {
            R -= bright;
          }
          else
          {
            R = 0;
          }
          if (G > 2 * bright)
          {
            G -= 2 * bright;
          }
          else
          {
            G = 0;
          }
          if (B > bright)
          {
            B -= bright;
          }
          else
          {
            B = 0;
          }
          usNewRGB = (R << 11) + (G << 5) + B;
        }
        
        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
      }
    }
  }
  
  /* Draws the text on the icon */
  {
    uint16_t len;
    uint16_t width;
    
    len = strlen(_tIcon->Text);
    
    if  (len == 0)
    {
      return;  /* No display If the length of the icon text is 0 */
    }
    
    /* Calculates the total width of the text */
    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
    {
      width = 6 * (len + _tFont->Space);
    }
    else  /* FC_ST_16 */
    {
      width = 8 * (len + _tFont->Space);
    }
    
    /* center horizontally */
    x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
    y = _tIcon->Top + _tIcon->Height + 2;
    LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_Blend565
*  Function discription: Pixel transparency mix color
*  Parameters: src : The original pixel
*            dst : mixed color
*          alpha : transparency 0-32
*  Return: void
*********************************************************************************************************
*/
uint16_t LCD_Blend565(uint16_t src, uint16_t dst, uint8_t alpha)
{
  uint32_t src2;
  uint32_t dst2;
  
  src2 = ((src << 16) |src) & 0x07E0F81F;
  dst2 = ((dst << 16) | dst) & 0x07E0F81F;
  dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
  return (uint16_t)((dst2 >> 16) | dst2);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawIcon32
*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
*  Parameters: _pIcon : Icon structure
*            _tFont : Font properties
*      _ucFocusMode : FocusMode.0 Represent normal icon  1Represents the selected icon
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
  const uint8_t *p;
  uint16_t      usRGB;
  int16_t       x, y;        /* Record relative coordinates within a window */
  uint8_t       R1,G1,B1,A;  /* New pixel color component */
  uint8_t       R0,G0,B0;    /* Previous pixel color component */
  
  p = (const uint8_t *)_tIcon->pBmp;
  p += 54;    /* Point directly to the image data area */
  
  /* Scan from left to right and bottom to top in BMP bitmap order */
  for (y = _tIcon->Height - 1; y >= 0; y--)
  {
    for (x = 0; x < _tIcon->Width; x++)
    {
      B1 = *p++;
      G1 = *p++;
      R1 = *p++;
      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque*/
      
      if (A == 0x00)  /* Need transparant, display back ground */
      {
        ;  /* Do not refresh background */
      }
      else if (A == 0xFF)  /* Completely opaque, display new pixels */
      {
        usRGB = RGB(R1, G1, B1);
        
        /* If selected */
        if (_ucFocusMode == 1)
        {
          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
        }
        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
      }
      else   /* semitransparent */
      {
        /* Actual color = foreground color * Alpha / 255 + Backgroundcolor * (255-Alpha) / 255 */
        //usRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
        usRGB = RGB(0,78,152);/* Fixed background */
        
        //usOldRGB = 0xFFFF;
        R0 = RGB565_R(usRGB);
        G0 = RGB565_G(usRGB);
        B0 = RGB565_B(usRGB);
        
        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
        usRGB = RGB(R1, G1, B1);
        
        /* If selected */
        if (_ucFocusMode == 1)
        {
          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
        }
        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
      }
    }
  }
  
  /* Draws the text on the icon */
  {
    usRGB = strlen(_tIcon->Text);
    
    if  (usRGB == 0)
    {
      return;  /* No display If the length of the icon text is 0 */
    }
    
    /* Calculates the total width of the text */
    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
    {
      usRGB = 6 * (usRGB + _tFont->Space);
    }
    else  /* FC_ST_16 */
    {
      usRGB = 8 * (usRGB + _tFont->Space);
    }
    
    /* center horizontally */
    x = (_tIcon->Left + _tIcon->Width / 2) - usRGB / 2;
    y = _tIcon->Top + _tIcon->Height + 2;
    
    /* If selected */
    if (_ucFocusMode == 1)
    {
      usRGB = _tFont->BackColor;
      _tFont->BackColor = CL_ORANGE;//LCD_Blend565(usNewRGB, CL_YELLOW, 10);
    }
    
    LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
    
    /* If selected */
    if (_ucFocusMode == 1)
    {
      _tFont->BackColor = usRGB;
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawIcon32_Ex
*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
*  Parameters: _pIcon : Icon structure
*            _tFont : Font properties
*      _ucFocusMode : Focus mode.0 Represent normal icon  1Represents the selected icon
*  Return: void
*********************************************************************************************************
*/
//This is a single buffer, with mutual exclusion, does not consider greater than 12288 bytes  
void LCD_DrawIcon32_Ex(const ICON_T_EX *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
//  const uint8_t *p;
//  uint16_t      usRGB;
//  int16_t       x, y;        /* Record relative coordinates within a window */
//  uint8_t       R1,G1,B1,A;  /* New pixel color component */
//  uint8_t       R0,G0,B0;    /* Previous pixel color component */
//  
//  uint32_t      Bytes;
//  
//  Bytes = _tIcon->Height * _tIcon->Width * 4 + 54;/* Number of bytes   */
//  
//  /* Permanently wait for SPI mutually exclusive resources */
//  os_mut_wait (&spimutex, 0xFFFF);
//  
//  if(Bytes <= 12288)
//  {
//    /* Read data with DMA */
//    sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + 54);/* Point directly to the image data area */
//    sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,Bytes);
//    sf_FastReadBuffer_Duad_IO_End_DMA();
//    p = (uint8_t *)s_spiBuf;
//  }
//  else
//  {
//    while(1);
//  }
//  
//  /* Scan from left to right and bottom to top in BMP bitmap order */
//  for (y = _tIcon->Height - 1; y >= 0; y--)
//  {
//    for (x = 0; x < _tIcon->Width; x++)
//    {
//      B1 = *p++;
//      G1 = *p++;
//      R1 = *p++;
//      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
//      
//      if (A == 0x00)  /* Need transparant, display back ground */
//      {
//        ;  /* Do not refresh background */
//      }
//      else if (A == 0xFF)  /* Completely opaque, display new pixels */
//      {
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//      else   /* semitransparent */
//      {
//        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
//        //usRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
//        usRGB = RGB(0,78,152);/* Fixed background */
//        
//        R0 = RGB565_R(usRGB);
//        G0 = RGB565_G(usRGB);
//        B0 = RGB565_B(usRGB);
//        
//        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
//        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
//        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//    }
//  }
//  
//  /* Release SPI mutually exclusive resources */
//  os_mut_release (&spimutex);
//  
//  /* Draws the text on the icon */
//  {
//    usRGB = strlen(_tIcon->Text);
//    
//    if  (usRGB == 0)
//    {
//      return;  /* No display If the length of the icon text is 0 */
//    }
//    
//    /* Calculates the total width of the text */
//    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
//    {
//      usRGB = 6 * (usRGB + _tFont->Space);
//    }
//    else  /* FC_ST_16 */
//    {
//      usRGB = 8 * (usRGB + _tFont->Space);
//    }
//    
//    /* center horizontally */
//    x = (_tIcon->Left + _tIcon->Width / 2) - usRGB / 2;
//    y = _tIcon->Top + _tIcon->Height + 2;
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      usRGB = _tFont->BackColor;
//      _tFont->BackColor = CL_ORANGE;//LCD_Blend565(usNewRGB, CL_YELLOW, 10);
//    }
//    
//    LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      _tFont->BackColor = usRGB;
//    }
//  }
}
////This is a single buffer, with mutual exclusion, considering greater than 12288 bytes  
///*
//*********************************************************************************************************
//*  Function name: LCD_DrawIcon32_Ex
//*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
//*  Parameters: _pIcon : Icon structure
//*            _tFont : Font properties
//*      _ucFocusMode : Focus mode.0 Represent normal icon  1Represents the selected icon
//*  Return: void
//*********************************************************************************************************
//*/
//void LCD_DrawIcon32_Ex(const ICON_T_EX *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
//{
//  const uint8_t *p;
//  uint16_t      usRGB;
//  int16_t       x, y;        /* Record relative coordinates within a window */
//  uint8_t       R1,G1,B1,A;  /* New pixel color component */
//  uint8_t       R0,G0,B0;    /* Previous pixel color component */
//  
//  uint32_t      Bytes;
//  uint32_t      NUM;
//  uint8_t       k = 0;
//  
//  Bytes = _tIcon->Height * _tIcon->Width * 4 + 54;/* Number of bytes   */
//  
//  /* Permanently wait for SPI mutually exclusive resources */
//  os_mut_wait (&spimutex, 0xFFFF);
//  
//  if(Bytes <= 12288)
//  {
//    /* Read data with DMA */
//    sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + 54);/* Point directly to the image data area */
//    sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,Bytes);
//    sf_FastReadBuffer_Duad_IO_End_DMA();
//    //sf_ReadBuffer((uint8_t *)s_spiBuf,_tIcon->Addr + 54,Bytes);
//    p = (uint8_t *)s_spiBuf;
//    NUM = 0xFFFFFFFF;
//  }
//  else
//  {
//    /* Read data with DMA */
//    sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + 54);/* Point directly to the image data area */
//    sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,12288);
//    sf_FastReadBuffer_Duad_IO_End_DMA();
//    //sf_ReadBuffer((uint8_t *)s_spiBuf,_tIcon->Addr + 54,12288);
//    p = (uint8_t *)s_spiBuf;
//    NUM = 12288;
//  }
//  
//  /* Scan from left to right and bottom to top in BMP bitmap order */
//  for (y = _tIcon->Height - 1; y >= 0; y--)
//  {
//    for (x = 0; x < _tIcon->Width; x++)
//    {
//      B1 = *p++;
//      G1 = *p++;
//      R1 = *p++;
//      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
//      
//      if(NUM != 0xFFFFFFFF)
//      {
//        NUM -= 4;
//        if(NUM == 0)
//        {
//          k ++;
//          Bytes -= 12288;
//          
//          /* Read data with DMA */
//          sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + (k * 12288) + 54);/* Point directly to the image data area */
//          if(Bytes >= 12288)
//          {
//            sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,12288);
//            //sf_ReadBuffer((uint8_t *)s_spiBuf,_tIcon->Addr + 54,12288);
//            NUM = 12288;
//          }
//          else
//          {
//            sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,Bytes);
//            //sf_ReadBuffer((uint8_t *)s_spiBuf,_tIcon->Addr + 54,Bytes);
//            NUM = Bytes;
//          }
//          sf_FastReadBuffer_Duad_IO_End_DMA();
//          p = (uint8_t *)s_spiBuf;
//        }
//      }
//      
//      if (A == 0x00)  /* Need transparant, display back ground */
//      {
//        ;  /* Do not refresh background */
//      }
//      else if (A == 0xFF)  /* Completely opaque, display new pixels */
//      {
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//      else   /* semitransparent */
//      {
//        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
//        //usRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
//        usRGB = RGB(0,78,152);/* Fixed background */
//        
//        R0 = RGB565_R(usRGB);
//        G0 = RGB565_G(usRGB);
//        B0 = RGB565_B(usRGB);
//        
//        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
//        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
//        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//    }
//  }
//  
//  /* Release SPI mutually exclusive resources */
//  os_mut_release (&spimutex);
//  
//  /* Draws the text on the icon */
//  {
//    usRGB = strlen(_tIcon->Text);
//    
//    if  (usRGB == 0)
//    {
//      return;  /* No display If the length of the icon text is 0 */
//    }
//    
//    /* Calculates the total width of the text */
//    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
//    {
//      usRGB = 6 * (usRGB + _tFont->Space);
//    }
//    else  /* FC_ST_16 */
//    {
//      usRGB = 8 * (usRGB + _tFont->Space);
//    }
//    
//    /* center horizontally */
//    x = (_tIcon->Left + _tIcon->Width / 2) - usRGB / 2;
//    y = _tIcon->Top + _tIcon->Height + 2;
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      usRGB = _tFont->BackColor;
//      _tFont->BackColor = CL_ORANGE;//LCD_Blend565(usNewRGB, CL_YELLOW, 10);
//    }
//    
//    LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      _tFont->BackColor = usRGB;
//    }
//  }
//}

////This is double buffered, with mutually exclusive, arbitrary size
///*
//*********************************************************************************************************
//*  Function name: LCD_DrawIcon32_Ex
//*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
//*  Parameters: _pIcon : Icon structure
//*            _tFont : Font properties
//*      _ucFocusMode : Focus mode.0 Represent normal icon  1Represents the selected icon
//*  Return: void
//*********************************************************************************************************
//*/
//void LCD_DrawIcon32_Ex(const ICON_T_EX *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
//{
//  const uint8_t *p;
//  uint16_t      usRGB;
//  int16_t       x, y;        /* Record relative coordinates within a window */
//  uint8_t       R1,G1,B1,A;  /* New pixel color component */
//  uint8_t       R0,G0,B0;    /* Previous pixel color component */
//  
//  uint16_t      NUM;
//  uint8_t       k = 0;
//  
//  NUM   = 0;
//  
//  /* Permanently wait for SPI mutually exclusive resources */
//  os_mut_wait (&spimutex, 0xFFFF);
//  
//  /* Read 4096 bytes of data using DMA double buffering */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + 54);
//  sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait(s_spiBuf,4096);
//  
//  /* Scan from left to right and bottom to top in BMP bitmap order */
//  for (y = _tIcon->Height - 1; y >= 0; y--)
//  {
//    for (x = 0; x < _tIcon->Width; x++)
//    {
//      if(NUM == 4096 || NUM == 0)
//      {
//        NUM = 4096;
//        
//        /* Wait for the last read to complete */
//        while(!dma_flag_get(DMA0,DMA_CH2, DMA_FLAG_FTF));
//        //while(!dma_flag_get(DMA0,DMA_CH1, DMA_FLAG_FTF));
//        
//        if((k % 2) == 0)
//        {
//          p = s_spiBuf;
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait(s_spiBuf1,4096);
//        }
//        else
//        {
//          p = s_spiBuf1;
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait(s_spiBuf,4096);
//        }
//        k++;
//      }
//      
//      B1 = *p++;
//      G1 = *p++;
//      R1 = *p++;
//      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
//      NUM -= 4;
//      
//      if (A == 0x00)  /* Need transparant, display back ground */
//      {
//        ;  /* Do not refresh background */
//      }
//      else if (A == 0xFF)  /* Completely opaque, display new pixels */
//      {
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//      else   /* semitransparent */
//      {
//        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
//        //usRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
//        usRGB = RGB(0,78,152);/* Fixed background */
//        
//        R0 = RGB565_R(usRGB);
//        G0 = RGB565_G(usRGB);
//        B0 = RGB565_B(usRGB);
//        
//        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
//        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
//        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//    }
//  }
//  
//  /* Wait for the last read to complete */
//  while(!dma_flag_get(DMA0,DMA_CH2, DMA_FLAG_FTF));
//  //while(!dma_flag_get(DMA0,DMA_CH1, DMA_FLAG_FTF));
//  sf_FastReadBuffer_Duad_IO_End_DMA();
//  
//  /* Release SPI mutually exclusive resources */
//  os_mut_release (&spimutex);
//  
//  /* Draws the text on the icon */
//  {
//    usRGB = strlen(_tIcon->Text);
//    
//    if  (usRGB == 0)
//    {
//      return;  /* No display If the length of the icon text is 0 */
//    }
//    
//    /* Calculates the total width of the text */
//    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
//    {
//      usRGB = 6 * (usRGB + _tFont->Space);
//    }
//    else  /* FC_ST_16 */
//    {
//      usRGB = 8 * (usRGB + _tFont->Space);
//    }
//    
//    /* center horizontally */
//    x = (_tIcon->Left + _tIcon->Width / 2) - usRGB / 2;
//    y = _tIcon->Top + _tIcon->Height + 2;
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      usRGB = _tFont->BackColor;
//      _tFont->BackColor = CL_ORANGE;//LCD_Blend565(usNewRGB, CL_YELLOW, 10);
//    }
//    
//    LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
//    
//    /* If selected */
//    if (_ucFocusMode == 1)
//    {
//      _tFont->BackColor = usRGB;
//    }
//  }
//}

//This is double-buffered,without mutex, size is arbitrary
/*
*********************************************************************************************************
*  Function name: LCD_DrawIcon32_Ex_NOmut
*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
*  Parameters: _pIcon : Icon structure
*            _tFont : Font properties
*      _ucFocusMode : Focus mode.0 Represent normal icon  1Represents the selected icon
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawIcon32_Ex_NOmut(const ICON_T_EX *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
//  const uint8_t *p;
//  uint16_t      usRGB;
//  int16_t       x, y;        /* Record relative coordinates within a window */
//  uint8_t       R1,G1,B1,A;  /* New pixel color component */
//  uint8_t       R0,G0,B0;    /* Previous pixel color component */
//  
//  uint16_t      NUM;
//  uint8_t       k = 0;
//  
//  NUM   = 0;
//  
//  /* Read 4096 bytes of data using DMA double buffering */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(_tIcon->Addr + 54);
//  sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf,4096);
//  
//  /* Scan from left to right and bottom to top in BMP bitmap order */
//  for (y = _tIcon->Height - 1; y >= 0; y--)
//  {
//    for (x = 0; x < _tIcon->Width; x++)
//    {
//      if(NUM == 4096 || NUM == 0)
//      {
//        NUM = 4096;
//        
//        /* Wait for the last read to complete */
//        sf_DMA_WaitForEnd();
//        //while(!dma_flag_get(DMA0,DMA_CH2, DMA_FLAG_FTF));
//        //while(!dma_flag_get(DMA0,DMA_CH1, DMA_FLAG_FTF));
//        
//        if((k % 2) == 0)
//        {
//          p = (uint8_t *)s_spiBuf;
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf1,4096);
//        }
//        else
//        {
//          p = (uint8_t *)s_spiBuf1;
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf,4096);
//        }
//        k++;
//      }
//      
//      B1 = *p++;
//      G1 = *p++;
//      R1 = *p++;
//      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
//      NUM -= 4;
//      
//      if (A == 0x00)  /* Need transparant, display back ground */
//      {
//        ;  /* Do not refresh background */
//      }
//      else if (A == 0xFF)  /* Completely opaque, display new pixels */
//      {
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//      else   /* semitransparent */
//      {
//        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
//        //usRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
//        usRGB = RGB(0,78,152);/* Fixed background */
//        
//        R0 = RGB565_R(usRGB);
//        G0 = RGB565_G(usRGB);
//        B0 = RGB565_B(usRGB);
//        
//        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
//        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
//        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_ucFocusMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_ORANGE, 10);
//        }
//        LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usRGB);
//      }
//    }
//  }
//  
//  /* Wait for the last read to complete */
//  sf_DMA_WaitForEnd();
//  //while(!dma_flag_get(DMA0,DMA_CH2, DMA_FLAG_FTF));
//  //while(!dma_flag_get(DMA0,DMA_CH1, DMA_FLAG_FTF));
//  sf_FastReadBuffer_Duad_IO_End_DMA();
//  
////  /* Draws the text on the icon */
////  {
////    usRGB = strlen(_tIcon->Text);
////    
////    if  (usRGB == 0)
////    {
////      return;  /* No display If the length of the icon text is 0 */
////    }
////    
////    /* Calculates the total width of the text */
////    if (_tFont->FontCode == FC_ST_12)    /* 12dot matrix */
////    {
////      usRGB = 6 * (usRGB + _tFont->Space);
////    }
////    else  /* FC_ST_16 */
////    {
////      usRGB = 8 * (usRGB + _tFont->Space);
////    }
////    
////    /* center horizontally */
////    x = (_tIcon->Left + _tIcon->Width / 2) - usRGB / 2;
////    y = _tIcon->Top + _tIcon->Height + 2;
////    
////    /* If selected */
////    if (_ucFocusMode == 1)
////    {
////      usRGB = _tFont->BackColor;
////      _tFont->BackColor = CL_ORANGE;//LCD_Blend565(usNewRGB, CL_YELLOW, 10);
////    }
////    
////    LCD_DispStrEx_NOmut(x, y, (char *)_tIcon->Text, _tFont, 0, 0);
////    
////    /* If selected */
////    if (_ucFocusMode == 1)
////    {
////      _tFont->BackColor = usRGB;
////    }
////  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawBmp16_NOmut
*  Function discription: Draw a 16-bit BMP on LCD  X start with 0 read 10 lines each time maximum 320*240 pixels supported
*  Parameters: _usX, _usY          : display coordinates
*            _usHeight, _usWidth : Height and width of the picture
*            _usAddr             : The storage address of the image in flash
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawBmp16_NOmut(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint32_t _usAddr)
{
//  const uint16_t *p;
//  uint32_t addr;
//  uint16_t c,cx;
//  uint8_t  k;
//  uint16_t NUM;
//  
//  LCD_WR_CMD(lcddev.setxcmd);
//  LCD_WR_DATA(_usX>>8);LCD_WR_DATA(_usX&0xFF);
//  LCD_WR_DATA((_usWidth+_usX)>>8);LCD_WR_DATA((_usWidth+_usX)&0xFF);
//  LCD_WR_CMD(lcddev.setycmd);
//  LCD_WR_DATA(_usY>>8);LCD_WR_DATA(_usY&0xFF);
//  LCD_WR_DATA((_usHeight+_usY)>>8);LCD_WR_DATA((_usHeight+_usY)&0xFF);
//  
//  LCD_WR_CMD(lcddev.wramcmd);
//  
//  k    = 0;
//  NUM  = 0;
//  addr = _usAddr;
//  
//  /* Read display data,use s_spiBuf/s_spiBuf1/s_spiBuf2/s_spiBuf3 as buffer */
//  
//  /* DMA double buffering is used to read data, reading 6400 bytes at a time, which is 10 lines of image data   */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(addr);
//  sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf,6400);
//  
//  for(c=0;c<_usHeight;c++)
//  {
//    for(cx=0;cx<_usWidth;cx++)
//    {
//      if(NUM == 0)
//      {
//        NUM = 6400;
//        
//        /* Wait for the last read to complete */
//        sf_DMA_WaitForEnd();
//      
//        if((k % 2) == 0)
//        {
//          p = (uint16_t *)s_spiBuf;
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)&s_spiBuf1[2304],6400);
//        }
//        else
//        {
//          p = (uint16_t *)&s_spiBuf1[2304];
//          sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf,6400);
//        }
//        k++;
//      }
//    #ifdef USE_ILI9341
//      TFTLCD->LCD_RAM = *p++;
//    #elif defined USE_ST7789V
//      TFTLCD->LCD_RAM = (*p >> 8);
//      TFTLCD->LCD_RAM =  *p;
//      p++;
//    #endif
//      
//      NUM -= 2;
//    }
//  }
//  
//  /* Wait for the last read to complete */
//  sf_DMA_WaitForEnd();
//  sf_FastReadBuffer_Duad_IO_End_DMA();
//  
//  
//  /* Set back to 320*240 */
//  LCD_WR_CMD(lcddev.setxcmd);
//  LCD_WR_DATA(0);LCD_WR_DATA(0);
//  LCD_WR_DATA(/*0x140>>8*/0x01);LCD_WR_DATA(/*0x140&0xFF*/0x40);
//  LCD_WR_CMD(lcddev.setycmd);
//  LCD_WR_DATA(0);LCD_WR_DATA(0);
//  LCD_WR_DATA(/*0xF0>>8*/0x00);LCD_WR_DATA(/*0xF0&0xFF*/0xF0);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawBmp16_EX_NOmut
*  Function discription: Draw a 16-bit BMP on LCD  120*40 pixels supported
*  Parameters: _usX, _usY          : display coordinates
*            _usHeight, _usWidth : Height and width of the picture
*            _usAddr             : The storage address of the image in flash
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawBmp16_EX_NOmut(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint32_t _usAddr)
{
//  uint8_t  k;
//  uint16_t NUM;
//  uint32_t addr;
//  uint32_t i;
//  uint8_t  *pbu;
//  const    uint16_t *p;
//  uint32_t ch;
//  
//  NUM  = _usWidth << 1;
//  addr = _usAddr;
//  pbu = (uint8_t *)s_spiBuf;
//  ch   = 0;
//  
//  /* Read display data,use s_spiBuf/s_spiBuf1/s_spiBuf2/s_spiBuf3 as buffer */
//  for(k=0;k<_usHeight;k++)
//  {
//    sf_FastReadBuffer_Duad_IO_Start_DMA(addr);
//    sf_FastReadBuffer_Duad_IO_Read_DMA_NoWait((uint8_t *)s_spiBuf3,NUM);
//    sf_DMA_WaitForEnd();
//    sf_FastReadBuffer_Duad_IO_End_DMA();
//    
//    for(i=0;i<NUM;i++)
//    {
//      pbu[ch++] = s_spiBuf3[i];
//    }
//    
//    addr += 640;/* Point to the bottom row of data  */
//  }
//  
//  LCD_WR_CMD(lcddev.setxcmd);
//  LCD_WR_DATA(_usX>>8);LCD_WR_DATA(_usX&0xFF);
//  LCD_WR_DATA((_usWidth+_usX-1)>>8);LCD_WR_DATA((_usWidth+_usX-1)&0xFF);
//  LCD_WR_CMD(lcddev.setycmd);
//  LCD_WR_DATA(_usY>>8);LCD_WR_DATA(_usY&0xFF);
//  LCD_WR_DATA((_usHeight+_usY)>>8);LCD_WR_DATA((_usHeight+_usY)&0xFF);
//  
//  LCD_WR_CMD(lcddev.wramcmd);
//  
//  p = (uint16_t *)s_spiBuf;
//  addr = _usWidth * _usHeight;
//  for(i=0;i<addr;i++)
//  {
//  #ifdef USE_ILI9341
//    TFTLCD->LCD_RAM = *p++;
//  #elif defined USE_ST7789V
//    TFTLCD->LCD_RAM = (*p >> 8);
//    TFTLCD->LCD_RAM =  *p;
//    p++;
//  #endif
//  }
//  
//  /* Set back to 320*240 */
//  LCD_WR_CMD(lcddev.setxcmd);
//  LCD_WR_DATA(0);LCD_WR_DATA(0);
//  LCD_WR_DATA(/*0x140>>8*/0x01);LCD_WR_DATA(/*0x140&0xFF*/0x40);
//  LCD_WR_CMD(lcddev.setycmd);
//  LCD_WR_DATA(0);LCD_WR_DATA(0);
//  LCD_WR_DATA(/*0xF0>>8*/0x00);LCD_WR_DATA(/*0xF0&0xFF*/0xF0);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawIcon32_16_16_NOmut
*  Function discription: Draw an icon on LCD, bitmap with transparent information (32-bit, RGBA).  Icon with text below  
*  Parameters: _usX, _usY          : display coordinates
*            _usHeight, _usWidth : Height and width of the picture
*            _usAddr             : The storage address of the image in flash
*            _usMode             : Display mode ,0:disconnect 1：connect
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawIcon32_16_16_NOmut(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint32_t _usAddr, uint8_t _usMode)
{
//  const uint8_t *p;
//  uint16_t      usRGB;
//  int16_t       x, y;        /* Record relative coordinates within a window */
//  uint8_t       R1,G1,B1,A;  /* New pixel color component */
//  uint8_t       R0,G0,B0;    /* Previous pixel color component */
//  
//  /* Permanently wait for SPI mutually exclusive resources */
//  //os_mut_wait (&spimutex, 0xFFFF);
//  
//  /* Read data with DMA */
//  sf_FastReadBuffer_Duad_IO_Start_DMA(_usAddr + 54);/* Point directly to the image data area */
//  sf_FastReadBuffer_Duad_IO_Read_DMA((uint8_t *)s_spiBuf,_usHeight*_usWidth*4);
//  sf_FastReadBuffer_Duad_IO_End_DMA();
//  p = (uint8_t *)s_spiBuf;
//  
//  /* Scan from left to right and bottom to top in BMP bitmap order */
//  for (y = _usHeight - 1; y >= 0; y--)
//  {
//    for (x = 0; x < _usWidth; x++)
//    {
//      B1 = *p++;
//      G1 = *p++;
//      R1 = *p++;
//      A  = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
//      
//      if (A == 0x00)  /* Need transparant, display back ground */
//      {
//        ;  /* Do not refresh background */
//      }
//      else if (A == 0xFF)  /* Completely opaque, display new pixels */
//      {
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_usMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_GREY, 25);
//        }
//        LCD_PutPixel(x + _usX, y + _usY, usRGB);
//      }
//      else   /* semitransparent */
//      {
//        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
//        //usRGB = LCD_GetPixel(x + _usX, y + _usY);
//        usRGB = RGB(0,78,152);/* Fixed background */
//        
//        R0 = RGB565_R(usRGB);
//        G0 = RGB565_G(usRGB);
//        B0 = RGB565_B(usRGB);
//        
//        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
//        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
//        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
//        usRGB = RGB(R1, G1, B1);
//        
//        /* If selected */
//        if (_usMode == 1)
//        {
//          usRGB = LCD_Blend565(usRGB, CL_GREY, 25);
//        }
//        LCD_PutPixel(x + _usX, y + _usY, usRGB);
//      }
//    }
//  }
//  
//  /* Release SPI mutually exclusive resources */
//  //os_mut_release (&spimutex);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawBmp32
*  Function discription: draw a 32-bit BMP map on LCD with transparent information bitmap (32-bit,RGBA)  
*  Parameters: _usX, _usY : display coordinates
*   _usHeight, _usWidth : Height and width of the picture
*                 _pBmp : Image data（with BMP file header ）
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawBmp32(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_pBmp)
{
  const uint8_t *p;
  uint16_t      usRGB;
  int16_t       x, y;        /* Record relative coordinates within a window */
  uint8_t       R1,G1,B1,A;  /* New pixel color component */
  uint8_t       R0,G0,B0;    /* Previous pixel color component */
  
  p = (const uint8_t *)_pBmp;
  p += 54;    /* Point directly to the image data area */
  
  /* Scan from left to Right and top to bottom in BMP bitmap order  */
  for (y = _usHeight - 1; y >= 0; y--)
  {
    for (x = 0; x < _usWidth; x++)
    {
      B1 = *p++;
      G1 = *p++;
      R1 = *p++;
      A = *p++;  /* Alpha value(transparency),0-255, 0 stands for transparant,1 stand for opaque */
      
      if (A == 0x00)  /* Need transparant, display back ground */
      {
        ;  /* Do not refresh background */
      }
      else if (A == 0xFF)  /* Completely opaque, display new pixels */
      {
        usRGB = RGB(R1, G1, B1);
        LCD_PutPixel(x + _usX, y + _usY, usRGB);
      }
      else   /* semitransparent */
      {
        /* Display color = foreground color * Alpha / 255 + Background color * (255-Alpha) / 255 */
        //usRGB = LCD_GetPixel(x + _usX, y + _usY);
        usRGB = RGB(0,78,152);/* Fixed background */
        
        R0 = RGB565_R(usRGB);
        G0 = RGB565_G(usRGB);
        B0 = RGB565_B(usRGB);
        
        R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
        G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
        B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
        usRGB = RGB(R1, G1, B1);
        LCD_PutPixel(x + _usX, y + _usY, usRGB);
      }
    }
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawLabel
*  Function discription: Draws a text label  
*  Parameters: _pLabel:Text label structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawLabel(LABEL_T *_pLabel)
{
  char dispbuf[256];
  
  uint8_t  i;
  uint16_t NewLen;
  
  NewLen = strlen(_pLabel->pCaption);
  
  if (NewLen > _pLabel->MaxLen)
  {
    LCD_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->pCaption, _pLabel->Font);
    _pLabel->MaxLen = NewLen;
  }
  else
  {
    for (i = 0; i < NewLen; i++)
    {
      dispbuf[i] = _pLabel->pCaption[i];
    }
    for (; i < _pLabel->MaxLen; i++)
    {
      dispbuf[i] = ' ';    /* Padding space at the end */
    }
    dispbuf[i] = 0;
    LCD_DispStr(_pLabel->Left, _pLabel->Top, dispbuf, _pLabel->Font);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawCheckBox
*  Function discription: Draw a checkbox
*  Parameters: _pCheckBox:Check the frame structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawCheckBox(CHECK_T *_pCheckBox)
{
  uint16_t x, y;
  
  /* Draw frame */
  x = _pCheckBox->Left;
  LCD_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
  LCD_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
  LCD_Fill_Rect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);
  
  /* Draw text labels */
  x = _pCheckBox->Left + CHECK_BOX_W + 2;
  y = _pCheckBox->Top + CHECK_BOX_H / 2 - 8;
  LCD_DispStr(x, y, _pCheckBox->pCaption, _pCheckBox->Font);
  
  if (_pCheckBox->Checked)
  {
    FONT_T font;
    
    font.FontCode = FC_ST_16;
    font.BackColor = CL_MASK;
    font.FrontColor = CHECK_BOX_CHECKED_COLOR;  /* color of check mark*/
    font.Space = 0;
    x = _pCheckBox->Left;
    LCD_DispStr(x + 3, _pCheckBox->Top + 3, (char *)"√", &font);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawEdit
*  Function discription: Draw an edit box on LCD
*  Parameters: _pEdit Edit the box structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawEdit(EDIT_T *_pEdit)
{
  uint16_t color;
  
  /* XP style edit box */
  LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
  LCD_DrawRect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BORDER_COLOR1);
  
  if(_pEdit->Focus)
    color = EDIT_ACTIVE_COLOR;
  else
    color = EDIT_BACK_COLOR;
  
  /* Fill background color */
  LCD_Fill_Rect(_pEdit->Left + 2, _pEdit->Top + 2, _pEdit->Height - 4, _pEdit->Width - 4, color);  /* background color Selected */
  
  /* center display */
  LCD_DispStrEx(_pEdit->Left+2, _pEdit->Top+(_pEdit->Height>>1)-6, _pEdit->pCaption, _pEdit->Font, _pEdit->Width - 4, ALIGN_CENTER);
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawEdit
*  Function discription: Draws a button on the LCD
*  Parameters: _pBtn:Pointer to button structure
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawButton(BUTTON_T *_pBtn)
{
  uint16_t x, y, h;
  
  h =  LCD_GetFontHeight(_pBtn->Font);
  x = _pBtn->Left + 3;
  y = _pBtn->Top + _pBtn->Height / 2 - h / 2;
  
  /* Save the background color temporarily */
  h = _pBtn->Font->BackColor;
  
  if (_pBtn->Focus == 1)
  {
    _pBtn->Font->BackColor = BUTTON_ACTIVE_COLOR;
  }
  else
  {
    /* The background color of the button is unified */
    _pBtn->Font->BackColor = BUTTON_BACK_COLOR;
  }
  
  /* XP style edit box */
  LCD_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
  LCD_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
  LCD_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);
  
  /* fill color */
  LCD_Fill_Rect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, _pBtn->Font->BackColor);  /* 选中后的底色 */
  
  /* center horizontally */
  LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width - 6, ALIGN_CENTER);
  
  /* Restore background color */
  _pBtn->Font->BackColor = h;
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawGroupBox
*  Function discription: Draw a groupbox on LCD
*  Parameters: _pBox:Groupbox structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawGroupBox(GROUP_T *_pBox)
{
  uint16_t x, y;
  
  /* Draw a shadow line */
  LCD_DrawRect(_pBox->Left + 1, _pBox->Top + 5, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER2);
  
  /* Draw the main frame line */
  LCD_DrawRect(_pBox->Left, _pBox->Top + 4, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER1);
  
  /*The group box title is displayed（Text at lefttop corner） */
  x = _pBox->Left + 9;
  y = _pBox->Top;
  LCD_DispStr(x, y, _pBox->pCaption, _pBox->Font);
}

/*
*********************************************************************************************************
*  Function name: LCD_DispControl
*  Function discription: Display control
*  Parameters: _pControl:Control pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DispControl(void *_pControl)
{
  uint8_t id;
  
  id = *(uint8_t *)_pControl;  /* Read ID */
  
  switch (id)
  {
    case ID_ICON:
      //void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
      break;
      
    case ID_WIN:
      LCD_DrawWin((WIN_T *)_pControl);
      break;
      
    case ID_LABEL:
      LCD_DrawLabel((LABEL_T *)_pControl);
      break;
      
    case ID_BUTTON:
      LCD_DrawButton((BUTTON_T *)_pControl);
      break;
      
    case ID_CHECK:
      LCD_DrawCheckBox((CHECK_T *)_pControl);
      break;
      
    case ID_EDIT:
      LCD_DrawEdit((EDIT_T *)_pControl);
      break;
      
    case ID_GROUP:
      LCD_DrawGroupBox((GROUP_T *)_pControl);
      break;
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_ButtonTouchDown
*  Function discription: Determine whether the button is pressed.  Check that the touch coordinates are within the range of the button.  Redraw the button.  
*  Parameters:  _btn : Button object
*        _usX, _usY: Touch coordinates
*  Return: 1 Within the range
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchDown(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
  if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
    && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
  {
    BUTTON_BEEP();  /* Key-touch beep */
    _btn->Focus = 1;
    LCD_DrawButton(_btn);
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_ButtonTouchRelease
*  Function discription: Determine whether the button is released.  Check that the touch coordinates are within the range of the button.  Redraw the button. 
*  Parameters:  _btn : Button object
*        _usX, _usY: Touch coordinates
*  Return: 1 Within the range
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchRelease(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
  _btn->Focus = 0;
  LCD_DrawButton(_btn);
  
  if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width)
    && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_InitButton
*  Function discription: Initializes the button structure.
*  Parameters:  _x, _y : coordinate
*             _h, _w : Height and width
*          _pCaption : button text
*             _pFont : button font
*  Return: void
*********************************************************************************************************
*/
void LCD_InitButton(BUTTON_T *_btn, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, char *_pCaption, FONT_T *_pFont)
{
  _btn->Left     = _x;
  _btn->Top      = _y;
  _btn->Height   = _h;
  _btn->Width    = _w;
  _btn->pCaption = _pCaption;
  _btn->Font     = _pFont;
  _btn->Focus    = 0;
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawProgressBar
*  Function discription: Draws a progress bar on the LCD
*  Parameters: _pbar:Progress bar structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawProgressBar(BAR_T *_pbar)
{
  uint16_t i,x;
  
  if(_pbar->Per == 0)
  {
    /* Draw the circles on both sides */
    /* Disc */
    LCD_FullCircle(_pbar->Left, _pbar->Top, _pbar->Height/2, BAR_BACK_COLOR);
    LCD_FullCircle(_pbar->Left+_pbar->Width-_pbar->Height, _pbar->Top, _pbar->Height/2, BAR_BACK_COLOR);
    /* Ring */
    LCD_DrawCircle(_pbar->Left, _pbar->Top, _pbar->Height/2, BAR_BORDER_COLOR);
    LCD_DrawCircle(_pbar->Left+_pbar->Width-_pbar->Height, _pbar->Top, _pbar->Height/2, BAR_BORDER_COLOR);
    /*Fill color   */
    LCD_Fill_Rect(_pbar->Left, _pbar->Top-_pbar->Height/2+1, _pbar->Height-1, _pbar->Width-_pbar->Height, BAR_BACK_COLOR);
    
    /*Draw straight border */
    LCD_Fill_Rect(_pbar->Left, _pbar->Top-_pbar->Height/2, 1, _pbar->Width-_pbar->Height, BAR_BORDER_COLOR);
    LCD_Fill_Rect(_pbar->Left, _pbar->Top+_pbar->Height/2, 1, _pbar->Width-_pbar->Height, BAR_BORDER_COLOR);
    
    /* Draw background */
    for(i=0;i<_pbar->Width-_pbar->Height;i++)
    {
      LCD_FullCircle(_pbar->Left+i, _pbar->Top, _pbar->Height/2-3, BAR_BODY_COLOR);
    }
    
    LCD_FullCircle(_pbar->Left, _pbar->Top, _pbar->Height/2-3, BAR_UPDATE_COLOR);
    
    return;
  }
  
  /* Update slider */
  for(i=1;i<_pbar->Per*_pbar->Pro;i++)
  {
    x = _pbar->Left+i;
    if(x>= (_pbar->Left + _pbar->Width-_pbar->Height))
    {
      return;
    }
    
    LCD_FullCircle(x, _pbar->Top,_pbar->Height/2-3, BAR_UPDATE_COLOR);
    LCD_PutPixel(x, _pbar->Top+_pbar->Height/2-3, BAR_UPDATE_COLOR);
  }
}

/*
*********************************************************************************************************
*  Function name: LCD_DrawDrop
*  Function discription: Draw a drop-down box on the LCD
*  Parameters: _pDrop:Drop-down box structure pointer
*  Return: void
*********************************************************************************************************
*/
void LCD_DrawDrop(DROP_BOX *_pDrop)
{
  uint16_t x, y, h;
  
  h =  LCD_GetFontHeight(_pDrop->Font);
  x = _pDrop->Left + 3;
  y = _pDrop->Top + _pDrop->Height / 2 - h / 2;
  
  /* Save the background color temporarily */
  h = _pDrop->Font->BackColor;
  
  if (_pDrop->Focus == 1)
  {
    _pDrop->Font->BackColor = DROP_ACTIVE_COLOR;
  }
  else
  {
    _pDrop->Font->BackColor = DROP_BACK_COLOR;
  }
    
  if(_pDrop->mode == 0)/* Drawing mode */
  {
    /* XP style edit box */
    LCD_DrawRect(_pDrop->Left, _pDrop->Top, _pDrop->Height, _pDrop->Width, DROP_BORDER_COLOR);
    LCD_DrawRect(_pDrop->Left + 1, _pDrop->Top + 1, _pDrop->Height - 2, _pDrop->Width - 2, DROP_BORDER_COLOR1);
    
    /* fill triangle base color  */
    LCD_Fill_Rect(_pDrop->Left + 1 + _pDrop->Width - _pDrop->Height, _pDrop->Top + 2, _pDrop->Height - 4, _pDrop->Height - 3, DROP_BORDER_COLOR2);
    
  //  /* Triangle dependent is not adaptive, so it matches the box of height 20 */
  //  /* Draw the line in the middle of the triangles */
  //  LCD_DrawLine(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
  //  _pDrop->Top + (_pDrop->Height>>1),\
  //  _pDrop->Left + _pDrop->Width - 5 ,\
  //  _pDrop->Top  + (_pDrop->Height>>1),DROP_BORDER_COLOR3);
  //  
  //  LCD_DrawLine(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
  //  _pDrop->Top + (_pDrop->Height>>1) -1,\
  //  _pDrop->Left + _pDrop->Width - 5 ,\
  //  _pDrop->Top  + (_pDrop->Height>>1) -1,DROP_BORDER_COLOR3);
  //  
  //  /* Draw a triangle */
  //  LCD_DrawTriangle_TOP(_pDrop->Left + _pDrop->Width - _pDrop->Height + (_pDrop->Height >> 1),\
  //  _pDrop->Top + 2,\
  //  ((_pDrop->Height+4) >> 2),1,DROP_BORDER_COLOR3);
  //  
  //  LCD_DrawTriangle_TOP(_pDrop->Left + _pDrop->Width - _pDrop->Height + (_pDrop->Height >> 1),\
  //  _pDrop->Top + 2 + 15,\
  //  ((_pDrop->Height+4) >> 2),-1,DROP_BORDER_COLOR3);
    
    
    /* This method is triangular adaptive */
    /* Draw the line in the middle of the triangles */
    LCD_DrawLine(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
    _pDrop->Top + (_pDrop->Height>>1) -1,\
    _pDrop->Left + _pDrop->Width - 5 ,\
    _pDrop->Top  + (_pDrop->Height>>1) -1,DROP_BORDER_COLOR3);
    
    LCD_DrawLine(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
    _pDrop->Top + (_pDrop->Height>>1),\
    _pDrop->Left + _pDrop->Width - 5 ,\
    _pDrop->Top  + (_pDrop->Height>>1),DROP_BORDER_COLOR3);
    
    /* Draw a triangle */
    LCD_DrawTriangle_BOTTOM(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
    _pDrop->Top + (_pDrop->Height>>1) -3,\
    (_pDrop->Height) -10,\
    1,\
    DROP_BORDER_COLOR3);
    
    LCD_DrawTriangle_BOTTOM(_pDrop->Left + 5 + _pDrop->Width - _pDrop->Height,\
    _pDrop->Top + (_pDrop->Height>>1) +2,\
    (_pDrop->Height) -10,\
    -1,\
    DROP_BORDER_COLOR3);
  
    /* Fill the base color*/
    LCD_Fill_Rect(_pDrop->Left + 2, _pDrop->Top + 2, _pDrop->Height - 4, _pDrop->Width - _pDrop->Height, _pDrop->Font->BackColor);  /* selected color */
  }
  
  /* center horizontally */
  LCD_DispStrEx(x-1, y, _pDrop->pCaption, _pDrop->Font, _pDrop->Width - 6 - _pDrop->Height, ALIGN_RIGHT);
  
  /* Restore background color */
  _pDrop->Font->BackColor = h;
}




/*
*********************************************************************************************************
*    Function name: LCD_DrawArc
*    Function discription: Draw an arc
*    Parameters:
*            _usX,_usY  ：Coordinates of the center of the circle
*            _usRadius  ：Radius of a circle 
*            _StartAng  : Start angle
*            _EndAng       : End angle
*            _usColor   : Arc color
*    Return: void
*********************************************************************************************************
*/
void LCD_DrawArc(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, float _StartRnd, float _EndRnd, uint16_t _usColor)
{
  float CurX, CurY, rnd;
  
  rnd = _StartRnd;
  while (rnd <= _EndRnd)
  {
    CurX = _usRadius * cos(rnd);
    CurY = _usRadius * sin(rnd);
    LCD_PutPixel(_usX + CurX, _usY - CurY, _usColor);
    rnd = rnd + 0.01f;
  }
}
/*
*********************************************************************************************************
*    Function name: LCD_DrawQuterCircle
*    Function discription: Draw a Quter Circle,the pen width is 1 pixel
*    Parameters:
*            _usX,_usY  : Coordinates of the center of the circle
*            _usRadius  : Radius of a circle 
*            _ucMode    : 0 for topleft, 1 for topright, 2 for bottom right, 3 for bottom left
*    Return: void
*********************************************************************************************************
*/
void LCD_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
  int32_t D;         /* Decision Variable */
  uint32_t CurX; /* Current X */
  uint32_t CurY; /* Current Y */
  
  D = 3 - (_usRadius << 1);
  
  CurX = 0;
  CurY = _usRadius;

  while (CurX <= CurY)
  {
    if (_ucMode == 0)
    {
      LCD_PutPixel(_usX - CurY, _usY - CurX, _usColor); // left -> top
      LCD_PutPixel(_usX - CurX, _usY - CurY, _usColor); // top -> left
    }
    else if (_ucMode == 1)
    {
      LCD_PutPixel(_usX + CurX, _usY - CurY, _usColor); // top -> Right
      LCD_PutPixel(_usX + CurY, _usY - CurX, _usColor); // Right -> top
    }
    else if (_ucMode == 2)
    {
      LCD_PutPixel(_usX + CurX, _usY + CurY, _usColor); // bottom -> Right
      LCD_PutPixel(_usX + CurY, _usY + CurX, _usColor); // Right -> bottom
    }
    else if (_ucMode == 3)
    {
      LCD_PutPixel(_usX - CurX, _usY + CurY, _usColor); // bottom -> left
      LCD_PutPixel(_usX - CurY, _usY + CurX, _usColor); // left -> bottom
    }
    
    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/*
*********************************************************************************************************
*    Function name: LCD_FillQuterCircle
*    Function discription: Fill a Quter Circle.
*    Parameters:
*            _usX,_usY  : Coordinates of the center of the circle
*            _usRadius  : Radius of a circle 
*            _usColor   : Fill color  
*            _ucMode    : 0 for lefttop 1 for Righttop  2 for leftbottom 3 for Rightbottom
*    Return: void
*********************************************************************************************************
*/
void LCD_FillQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
  int32_t D;
  uint32_t CurX; /* Current X */
  uint32_t CurY; /* Current Y */
  
  D = 3 - (_usRadius << 1);
  CurX = 0;
  CurY = _usRadius;
  
  while (CurX <= CurY)
  {
    if (_ucMode == 0)
    {
      LCD_DrawLine(_usX - CurY, _usY - CurX, _usX, _usY - CurX, _usColor); // left -> top
      LCD_DrawLine(_usX - CurX, _usY - CurY, _usX, _usY - CurY, _usColor); // top -> left
    }
    else if (_ucMode == 1)
    {
      LCD_DrawLine(_usX + CurX, _usY - CurY, _usX, _usY - CurY, _usColor); // top -> Right
      LCD_DrawLine(_usX + CurY, _usY - CurX, _usX, _usY - CurX, _usColor); // Right -> top
    }
    else if (_ucMode == 2)
    {
      LCD_DrawLine(_usX + CurX, _usY + CurY, _usX, _usY + CurY, _usColor); // bottom -> Right
      LCD_DrawLine(_usX + CurY, _usY + CurX, _usX, _usY + CurX, _usColor); // Right -> bottom
    }
    else if (_ucMode == 3)
    {
      LCD_DrawLine(_usX - CurX, _usY + CurY, _usX, _usY + CurY, _usColor); // bottom -> left
      LCD_DrawLine(_usX - CurY, _usY + CurX, _usX, _usY + CurX, _usColor); // left -> bottom
    }
    
    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/*
*********************************************************************************************************
*    Function name: LCD_DrawRoundRect
*    Function discription: Draw rounded rectangle
*    Parameters:
*            _usX,_usY:Coordinates of the top left corner of the rectangle
*            _usHeight :Height of rectangle
*            _usWidth  :Width of rectangle
*            _usArc    :Radius of the arc of the rounded corner
*            _usColor  :color
*    Return: void
*********************************************************************************************************
*/
void LCD_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,uint16_t _usRadius, uint16_t _usColor)
{
  if (_usHeight < 2 * _usRadius)
  {
    _usHeight = 2 * _usRadius;
  }
  
  if (_usWidth < 2 * _usRadius)
  {
    _usWidth = 2 * _usRadius;
  }
  
  LCD_DrawQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0); /* lefttop角的弧 */
  LCD_DrawLine(_usX + _usRadius, _usY, _usX + _usWidth - _usRadius - 1, _usY, _usColor);
  
  LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1); /* Righttop角的弧 */
  LCD_DrawLine(_usX + _usWidth - 1, _usY + _usRadius, _usX + _usWidth - 1, _usY + _usHeight - _usRadius - 1, _usColor);
  
  LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2); /* Rightbottom角的弧 */
  LCD_DrawLine(_usX + _usRadius, _usY + _usHeight - 1, _usX + _usWidth - _usRadius - 1, _usY + _usHeight - 1, _usColor);
  
  LCD_DrawQuterCircle(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3); /* leftbottom角的弧 */
  LCD_DrawLine(_usX, _usY + _usRadius, _usX, _usY + _usHeight - _usRadius - 1, _usColor);
}

/*
*********************************************************************************************************
*    Function name: LCD_FillRoundRect
*    Function discription: Fill rounded rectangle
*    Parameters:
*            _usX,_usY:Coordinates of the top left corner of the rectangle
*            _usHeight :Height of rectangle
*            _usWidth  :Width of rectangle
*            _usArc    :Radius of the arc of the rounded corner
*            _usColor  :color
*    Return: void
*********************************************************************************************************
*/
void LCD_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,uint16_t _usRadius, uint16_t _usColor)
{        
  if (_usHeight < 2 * _usRadius)
  {
    _usHeight = 2 * _usRadius;
  }

  if (_usWidth < 2 * _usRadius)
  {
    _usWidth = 2 * _usRadius;
  }
  
  LCD_FillQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0); /* Arc of the angle lefttop */
  
  LCD_Fill_Rect(_usX + _usRadius, _usY, _usRadius + 1, _usWidth - 2 * _usRadius, _usColor);
  
  LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1); /* Arc of the angle Righttop */
  
  LCD_Fill_Rect(_usX, _usY + _usRadius, _usHeight - 2 * _usRadius, _usWidth, _usColor);
  
  LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2); /* Arc of the angle Rightbottom */
  
  LCD_Fill_Rect(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius + 1, _usWidth - 2 * _usRadius, _usColor);
  
  LCD_FillQuterCircle(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3); /* Arc of the angle leftbottom */
}

/*
*********************************************************************************************************
*    Function name: DispLabelRound
*    Function discription: Display Label Round,for DAP-LINK
*    Parameters: _usX : x coordinate
*              _usY : Y coordinate
*              _usHeight : Height
*              _usWidth : Width
*              _usColor :Fill color  
*              _pStr : Display content
*              _pFont : Font
*    Return: void
*********************************************************************************************************
*/
void DispLabelRound(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,uint16_t _usColor, char *_pStr, FONT_T *_tFont)
{
  uint16_t fh;
  uint16_t offset;
 
  /* Draw rounded rectangles */
  LCD_FillRoundRect(_usX, _usY, _usHeight, _usWidth, 5,_usColor);
  
  fh = LCD_GetFontHeight(_tFont);
  if (_usHeight > fh)
  {
    offset = (_usHeight - fh) / 2;
  }
  else
  {
    offset = 0;
  }
  LCD_DispStrEx(_usX, _usY + offset, _pStr, _tFont, _usWidth, ALIGN_CENTER);
}

/*
int fi1=0;
int fi2=0;
int fi3=0;
int fi4=0;
void arc_chabu_area1(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u8 width,u16 point_color)
{
  u16 x,y,xi,yi,color;
  u8 e;
  //color=POINT_COLOR;
  //POINT_COLOR=point_color;
  x=xi=x2;
  y=yi=y2;
  e=abs(x2-x1)+abs(y2-y1);
  while(e!=0)
  {
    if(fi1>=0)
    {
       x=xi-1;
       fi1=fi1+2*(x0-xi)+1;
        LCD_DrawLine(xi,yi,x,y,point_color);
       xi=x;
       e--;
    } 
    else
    {
      y=yi-1;
      fi1=fi1+2*(y0-yi)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
      yi=y;
      e--;
    }  
  }
  
  //POINT_COLOR=color;
 
}
 
void arc_chabu_area2(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u8 width,u16 point_color)
{
  
  u16 x,y,xi,yi,color;
  u8 e;
  //color=POINT_COLOR;
 
  //POINT_COLOR=point_color;
 
  x=xi=x2;
  y=yi=y2;
  e=abs(x2-x1)+abs(y2-y1);
  while(e!=0)
  {
    if(fi4>=0)
    {
       y=yi-1;
       fi4=fi4+2*(y0-yi)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
       yi=y;
       e--;
    } 
    else
    {
      x=xi+1;
      fi4=fi4+2*(xi-x0)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
      xi=x;
      e--;
    }  
  }
 
  //POINT_COLOR=color;
 
}
 
void arc_chabu_area3(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u8 width,u16 point_color)
{
  
  u16 x,y,xi,yi,color;
  u8 e;
 
  //color=POINT_COLOR;
  //POINT_COLOR=point_color;
 
  x=xi=x2;
  y=yi=y2;
  e=abs(x2-x1)+abs(y2-y1);
  while(e!=0)
  {
    if(fi3>=0)
    {
       x=xi+1;
       fi3=fi3+2*(xi-x0)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
       xi=x;
       e--;
    } 
    else
    {
      y=yi+1;
      fi3=fi3+2*(yi-y0)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
      yi=y;
      e--;
    }  
  }
  
  //POINT_COLOR=color;
 
}

void arc_chabu_area4(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u8 width,u16 point_color)
{
  
  u16 x,y,xi,yi,color;
  u8 e;
  //color=POINT_COLOR;
  //POINT_COLOR=point_color;
  x=xi=x2;
  y=yi=y2;
  e=abs(x2-x1)+abs(y2-y1);
  while(e!=0)
  {
    if(fi2>=0)
    {
       y=yi+1;
       fi2=fi2+2*(yi-y0)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
       yi=y;
       e--;
    } 
    else
    {
      x=xi-1;
      fi2=fi2+2*(x0-xi)+1;
      LCD_DrawLine(xi,yi,x,y,point_color);
      xi=x;
      e--;
    }  
  }
  
  //POINT_COLOR=color;  
 
}
*/


