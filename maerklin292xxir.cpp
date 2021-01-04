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

static uint16_t codeCache[35];
static const uint16_t kIrLed = MAERKLIN292XXIR_IR_PIN; 
static IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
static const stc_maerklin_292xx_ir_channelset_t stcChannelSetA = {
  {0x77,0x77}, //stop
  {0x1e59,0xe9b3}, //backward
  {0x95e1,0x315d}, //forward  
  {0x55,0x55}, //light
  {0xdd,0xdd}, //sound1
  {0xbb,0xbb}, //sound2 
  {0x33,0x33}  //sound3 
};

static const stc_maerklin_292xx_ir_channelset_t stcChannelSetB = {
  {0xee,0xee}, //stop     
  {0xd731,0x935d}, //backward 
  {0x15bd,0x5e73}, //forward
  {0xd1,0xd1}, //light 
  {0x59,0x59}, //sound1 
  {0x11,0x11}, //sound2 
  {0x99,0x99}  //sound3 
};

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
  static uint8_t u8LastCommand2 = 0;
  uint8_t u8Command = 0;
  uint8_t u8Command2 = 0;
  static stc_maerklin_292xx_ir_codeset_t* pLastCodeSet = 0;
  int i;
  
  if ((enAddress & 0xf0) == 0)
  {
      codeCache[0] = 5400;
      codeCache[17] = 10000;
      codeCache[18] = 5400;
      u8Command = (((uint8_t)enFunction & 0x7) << 4) | ((uint8_t)enAddress & 0xf);
      
      if (u8Command != u8LastCommand)
      {
        bToggle = false;
      }
      u8LastCommand = u8Command;
      
      if (bToggle)
      {
        u8Command |= 0x80;
      }
      
  } else
  {
      codeCache[0] = 20000;
      codeCache[17] = 45000;
      codeCache[18] = 20000;
      stc_maerklin_292xx_ir_channelset_t* pstcChannelSet = 0;
      stc_maerklin_292xx_ir_codeset_t* pstcCodeSet = 0;
      if (enAddress == enMaerklin292xxIrAddressA)
      {
          pstcChannelSet = (stc_maerklin_292xx_ir_channelset_t*)&stcChannelSetA;
      } else if (enAddress == enMaerklin292xxIrAddressB)
      {
          pstcChannelSet = (stc_maerklin_292xx_ir_channelset_t*)&stcChannelSetB;
      } else
      {
          return;
      }
      switch(enFunction)
      {
        case enMaerklin292xxIrFuncStop:
          pstcCodeSet = &pstcChannelSet->stop;
          break;
        case enMaerklin292xxIrFuncBackward:
          pstcCodeSet = &pstcChannelSet->backward;
          break;
        case enMaerklin292xxIrFuncForward:
          pstcCodeSet = &pstcChannelSet->forward;
          break;
        case enMaerklin292xxIrFuncLight:
          pstcCodeSet = &pstcChannelSet->light;
          break;
        case enMaerklin292xxIrFuncSound1:
          pstcCodeSet = &pstcChannelSet->sound1;
          break;
        case enMaerklin292xxIrFuncSound2:
          pstcCodeSet = &pstcChannelSet->sound2;
          break;
        case enMaerklin292xxIrFuncSound3:
          pstcCodeSet = &pstcChannelSet->sound3;
          break;
      }
      if (pstcCodeSet == 0) return;
      if (pLastCodeSet != pstcCodeSet)
      {
         bToggle = false;
         pLastCodeSet = pstcCodeSet;
      }
      if (bToggle)
      {
        u8Command = pstcCodeSet->codes[1] & 0xff;
        u8Command2 = (pstcCodeSet->codes[1] >> 8) & 0xff;
      } else {
        u8Command = pstcCodeSet->codes[0] & 0xff;
        u8Command2 = (pstcCodeSet->codes[0] >> 8) & 0xff;
      }
      
  }
  
    
  for(i = 0;i < 8;i++)
  {
    if ((u8Command & 0x80) != 0) 
    {
      codeCache[i*2 + 1] = 1700;
      codeCache[i*2 + 2] = 600;
      codeCache[i*2 + 19] = 1700;
      codeCache[i*2 + 20] = 600;
    } else {
      codeCache[i*2 + 1] = 600;
      codeCache[i*2 + 2] = 1700;
      codeCache[i*2 + 19] = 600;
      codeCache[i*2 + 20] = 1700;
    }
    u8Command = u8Command << 1;
  }
  if (u8Command2 != 0)
  {
    for(i = 0;i < 8;i++)
    {
      if ((u8Command2 & 0x80) != 0) 
      {
        codeCache[i*2 + 19] = 1700;
        codeCache[i*2 + 20] = 600;
      } else {
        codeCache[i*2 + 19] = 600;
        codeCache[i*2 + 20] = 1700;
      }
      u8Command2 = u8Command2 << 1;
    }
  }
  irsend.sendRaw(codeCache,35,38);
  delay(10);
  if ((enAddress & 0xf0) != 0)
  {
    delay(200);
  }
  bToggle = !bToggle;
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
