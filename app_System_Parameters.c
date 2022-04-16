/**
  ******************************************************************************
  * @file    app_System_Parameters.c
  * @author  COUNS Team
  * @version V1.0
  * @date    22-March-2019
  * @brief   app_System_Parameters files
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

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Extern variables reference-------------------------------------------------*/

/* Variable definition -------------------------------------------------------*/
PARAM_T    g_tParam;

/*
*********************************************************************************************************
*  Function name: LoadParam
*  Function description: read parameters from Flash to g_tParam
*  Formal parameters: none
*  return value: none
*********************************************************************************************************
*/
void LoadParam(void)
{
  /* Read the parameters in Flash -- double insurance */
  bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_8,(uint8_t *)&g_tParam,sizeof(PARAM_T));
  if (g_tParam.ParamVer != PARAM_VER)
  {
    bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_8,(uint8_t *)&g_tParam,sizeof(PARAM_T));
    if (g_tParam.ParamVer != PARAM_VER)
    {
      g_tParam.Switch_para = 2;
      
      /* Read the parameters in Flash -- double insurance */
      bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_9,(uint8_t *)&g_tParam,sizeof(PARAM_T));
      if (g_tParam.ParamVer != PARAM_VER)
      {
        bsp_ReadCpuFlash(ADDR_FLASH_SECTOR_9,(uint8_t *)&g_tParam,sizeof(PARAM_T));
        if (g_tParam.ParamVer != PARAM_VER)
        {
          /* Read the parameters in Flash -- double insurance */
          g_tParam.Switch_para = 1;
          
          /* If you use it for the first time or you need to reset the parameter area, erase all the system parameter storage area */
          
        }
      }
    }
  }
  
  /* Fill default parameters */
  if (g_tParam.ParamVer != PARAM_VER)
  {
    /* reset */
    Recovery_of_factory_parameters(1);
    
    /* Write new system parameters to Flash */
    SaveParam(0);
  }
  
  /* The machine model and the date of the machine version do not change with the system parameters */
  memcpy (g_tParam.Model_number, (uint8_t *)Model_Number, 14);
  memcpy (g_tParam.Version_date, (uint8_t *)Version_Date, 15);
  
  /* Calculate the machine serial number */
  App_Calculate_SN(g_tParam.Serial_number);
}

/*
*********************************************************************************************************
*  Function name: SaveParam
*  Function description: Write the global variable g_tParam into the Flash
*  Formal parameters: mode:0: restore system parameters 1: restore factory settings
*  return value: none
*********************************************************************************************************
*/
void Recovery_of_factory_parameters(const uint8_t mode)
{
  /* The machine model and the date of the machine version do not change with the system parameters */
  memcpy (g_tParam.Model_number, (uint8_t *)Model_Number, 14);
  memcpy (g_tParam.Version_date, (uint8_t *)Version_Date, 15);
  
  /* motherboard version number */
  memcpy (g_tParam.Master_Version, (uint8_t *)"V0.00.00", 9);
  
  
  /* LCD backlight brightness, 100 means the brightest, 0 means the darkest */
  /* This parameter represents 100%, the valid range (10-100） */
  g_tParam.ucBackLight = 80;
  
  /* Calculate the machine serial number */
  App_Calculate_SN(g_tParam.Serial_number);
  
  
  
  /* temperature threshold */
  g_tParam.Temperature_threshold = 32;
  
  /* heart rate threshold */
  g_tParam.Heart_rate_threshold = 80;
  
  /* speed threshold */
  g_tParam.Speed_threshold = 3600;
  
  
  
  /* System parameters start from the first sector */
  g_tParam.Switch_para = 1;
  
  /* Parameter area version control */
  g_tParam.ParamVer = PARAM_VER;
}

/*
*********************************************************************************************************
*  Function name: SaveParam
*  Function description: Write the global variable g_tParam into the Flash
*  Formal parameters: dly: choose whether to call the system delay until the erasing is completed
*  return value: none
*********************************************************************************************************
*/
void SaveParam(const uint8_t dly)
{
  /* System parameter storage */
  if(g_tParam.Switch_para == 1)
  {
    g_tParam.Switch_para = 2;
    
    bsp_WriteCpuFlash(ADDR_FLASH_SECTOR_8,(uint8_t *)&g_tParam,sizeof(PARAM_T));
    
    __set_PRIMASK(1);      /* Turn off interrupts */
    /* FLASH unlock */
    FLASH_Unlock();
    
    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    
    FLASH_EraseSector(bsp_GetSector(ADDR_FLASH_SECTOR_9), VoltageRange_3);
    /* Flash Lock, prohibit writing to the Flash control register */
    FLASH_Lock();
    __set_PRIMASK(0);      /* open interrupt */
  }
  else if(g_tParam.Switch_para == 2)
  {
    g_tParam.Switch_para = 1;
    
    bsp_WriteCpuFlash(ADDR_FLASH_SECTOR_9,(uint8_t *)&g_tParam,sizeof(PARAM_T));
    
    __set_PRIMASK(1);      /* Turn off interrupts */
    /* FLASH unlock */
    FLASH_Unlock();
    
    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    
    FLASH_EraseSector(bsp_GetSector(ADDR_FLASH_SECTOR_8), VoltageRange_3);
    /* Flash Lock, prohibit writing to the Flash control register */
    FLASH_Lock();
    __set_PRIMASK(0);      /* open interrupt */
  }
  else
  {
    while(1);//assertion
  }
}

/****** Copyright (C),2001-2017 COUNS 高优 Inc.2018 COUNS Team ****** END OF FILE ******/
