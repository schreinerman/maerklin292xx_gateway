/**
 *******************************************************************************
 ** Created by Manuel Schreiner
 **
 ** Copyright © 2021 io-expert.com. All rights reserved.
 **
 ** 1. Redistributions of source code must retain the above copyright notice,
 **    this condition and the following disclaimer.
 **
 ** This software is provided by the copyright holder and contributors "AS IS"
 ** and any warranties related to this software are DISCLAIMED.
 ** The copyright owner or contributors be NOT LIABLE for any damages caused
 ** by use of this software.

 *******************************************************************************
 */

/**
 *******************************************************************************
 **\file locodatabase.c
 **
 ** Loco Database
 ** A detailed description is available at
 ** @link LocoDatabaseGroup file description @endlink
 **
 ** History:
 ** - 2021-2-21  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __LOCODATABASE_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */


#include "locodatabase.h"
#include "../withrottle/withrottle.h"
#include "maerklin292xxir.h"

/**
 *******************************************************************************
 ** Local pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Global variable definitions (declared in header file with 'extern') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Local type definitions ('typedef') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Local variable definitions ('static') 
 *******************************************************************************
 */

static stc_withrottle_loco_listitem_t locos[10];
static int speedstatus[10];

/**
 *******************************************************************************
 ** Local function prototypes ('static') 
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Function implementation - global ('extern') and local ('static') 
 *******************************************************************************
 */

/**
 * Callback handler called if speed had changed
 * 
 * \param pHandle loco handle
 * 
 * \param bForward foreard = true, backward = false
 * 
 * \praram speed speed between 0 and 126
 */
static void locoSpeed(stc_withrottle_loco_t* pHandle, bool bForward, uint8_t speed)
{
  int iSpeed = speed + 41;
  
  if (!bForward)
  {
    iSpeed = -iSpeed / 42;
  } else
  {
    iSpeed = iSpeed / 42;
  }
  
  Serial.print("Loco ");
  Serial.print(pHandle->u32Address);
  Serial.print(" Speed Updated: ");
  Serial.println(iSpeed);
  if ((iSpeed != speedstatus[pHandle->u32Address]) || (iSpeed == 0))
  {
      speedstatus[pHandle->u32Address] = iSpeed;
      Maerklin292xxIr_SetSpeed((en_maerklin_292xx_ir_address_t)pHandle->u32Address,iSpeed);
  }
  
}

/**
 * Callback handler called if function was toggled
 * 
 * \param pHandle loco handle
 * 
 * \param u8Function function between 0...28
 */
static bool locoFunction(stc_withrottle_loco_t* pHandle, uint8_t u8Function)
{
  Serial.print("Loco ");
  Serial.print(pHandle->u32Address);
  Serial.print(" Function Updated: ");
  Serial.println(u8Function);
  pHandle->u32FunctionMask ^= (1 << u8Function);
  switch(u8Function)
  {
    case 0:
      Maerklin292xxIr_ToggleSoundLight((en_maerklin_292xx_ir_address_t)pHandle->u32Address,enMaerklin292xxIrFuncLight);
      break;
    case 1:
      Maerklin292xxIr_ToggleSoundLight((en_maerklin_292xx_ir_address_t)pHandle->u32Address,enMaerklin292xxIrFuncSound1);
      break;
    case 2:
      Maerklin292xxIr_ToggleSoundLight((en_maerklin_292xx_ir_address_t)pHandle->u32Address,enMaerklin292xxIrFuncSound2);
      break;
    case 3:
      Maerklin292xxIr_ToggleSoundLight((en_maerklin_292xx_ir_address_t)pHandle->u32Address,enMaerklin292xxIrFuncSound3);
      break;
  }
  return true;
}

/**
 * Init database
 * 
 * A = address 1, Z = address 26, addresses 1-10 are supported
 */
void LocoDatabase_Init(void)
{
  for(int i = 0;i < 10;i++)
  {
    locos[i].locoItem.bLongAddress = false;
    locos[i].locoItem.u32Address = i + 1;
    locos[i].locoItem.cbSpeedUpdated = locoSpeed;
    locos[i].locoItem.cbFunctionUpdated = locoFunction;
    locos[i].locoItem.bDir = true;
    locos[i].locoItem.speed = 0;
    locos[i].locoItem.u32FunctionMask = 0;
    locos[i].pNextItem = NULL;
    speedstatus[i] = 0;
    WiThrottle_AddLoco(&locos[i]);
  }
}



/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
