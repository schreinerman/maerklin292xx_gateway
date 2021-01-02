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
 **\file maerklin292xxir.c
 **
 ** Maerklin292xx IR protocol using IRSend
 ** A detailed description is available at
 ** @link Maerklin292xxIrGroup file description @endlink
 **
 ** History:
 ** - 2021-1-2  1.00  Manuel Schreiner
 *******************************************************************************
 */

#define __MAERKLIN292XXIR_C__

/**
 *******************************************************************************
 ** Include files
 *******************************************************************************
 */


#include <Arduino.h>
#include "maerklin292xxir.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

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

static uint16_t codeCache[17];
static const uint16_t kIrLed = MAERKLIN292XXIR_IR_PIN; 
static IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

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

/*
 * Init IR Functionality based on IRremote
 */
void Maerklin292xxIr_Init(void)
{
  //initiate IR library
  irsend.begin(); 
}

/*
 * Send data
 * 
 * \param enAddress  Address, can be enMaerklin292xxIrAddressC or enMaerklin292xxIrAddressD
 * 
 * \param enFunction Function, can be one of en_maerklin_292xx_ir_func_t defined in maerklin292xxir.h
 */
void Maerklin292xxIr_Send(en_maerklin_292xx_ir_address_t enAddress, en_maerklin_292xx_ir_func_t enFunction)
{
  static bool bToggle = false;
  static uint8_t u8LastCommand = 0;
  uint8_t u8Command = (((uint8_t)enFunction & 0x7) << 4) | ((uint8_t)enAddress & 0xf);
  int i;
  if (u8Command != u8LastCommand)
  {
    bToggle = false;
  }
  u8LastCommand = u8Command;
  
  if (bToggle)
  {
    u8Command |= 0x80;
  }
  codeCache[0] = 5400;
  
  for(i = 0;i < 8;i++)
  {
    if ((u8Command & 0x80) != 0) 
    {
      codeCache[i*2 + 1] = 1700;
      codeCache[i*2 + 2] = 600;
    } else {
      codeCache[i*2 + 1] = 600;
      codeCache[i*2 + 2] = 1700;
    }
    u8Command = u8Command << 1;
  }
  irsend.sendRaw(codeCache,17,38);
  delay(10);
  bToggle = !bToggle;
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
