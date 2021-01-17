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
 ** Protocols:
 **
 ** locomotives addresses A-B:
 ** ==========================
 **
 **  _____             ......... _____             .........
 ** |  S  | 1st 8-bits . delay .|  S  | 2nd 8-bits . delay .
 ** |     |____________.........|     |____________.........
 **  20ms    2.5ms*8      45ms    20ms    2.5ms*8    200ms
 **
 ** |<---- High ---->|   |<---- Low  ---->|
 **  _____                 __________
 ** |     |               |          | 
 ** |     |__________     |          |_____
 **  0.6ms    1.7ms           1.7ms   0.6ms
 **
 ** locomotives addresses C-D:
 ** ==========================
 **
 **  _____             .........
 ** |  S  |   8-bits   . delay .
 ** |     |____________.........
 **  5.4ms    2.5ms*8    50ms
 **
 **  bit   description
 **  7     toggle bit
 **  6..4  command
 **  3..0  address, C: 0b0101, D: 0b1010
 **
 ** |<---- High ---->|   |<---- Low  ---->|
 **  _____                 __________
 ** |     |               |          | 
 ** |     |__________     |          |_____
 **  0.6ms    1.7ms           1.7ms   0.6ms
 **
 ** locomotives addresses G-H:
 ** ==========================
 **  _____                              .........
 ** |  S  |   7-bits + 7-bits inverted  . delay .
 ** |     |_____________________________.........
 **  4.1ms              ...             min. 20ms
 **
 **  bit   description
 **  6     address 0 = G, 1 = H
 **  5..3  speed
 **  2..0  function
 **
 ** |<---- Low ---->|      |<---- High  ---->|
 **  ______                 __________
 ** |      |               |          | 
 ** |      |_______        |          |_____
 **  0.5ms   0.5ms           1.5ms    0.5ms
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

static uint16_t codeCache[100];
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

static uint8_t au8LastStates[26];

static en_maerklin_292xx_ir_address_t enLastAddress = enMaerklin292xxIrAddressA;
static volatile uint8_t u8Repeat = 0;
static volatile uint32_t u32LastUpdate = 0;
static volatile uint32_t u32UpdateRate = 1000;
static bool debugMode = true;

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
 * \param enAddress  Address, can be enMaerklin292xxIrAddressA...H
 * 
 * \param enFunction Function, can be one of en_maerklin_292xx_ir_func_t defined in maerklin292xxir.h
 */
void Maerklin292xxIr_Send(en_maerklin_292xx_ir_address_t enAddress, uint8_t enFunction)
{
  static bool bToggle = false;
  static uint8_t u8CommandLen = 0;
  static uint8_t u8LastCommand = 0;
  static uint8_t u8LastCommand2 = 0;
  static uint16_t u16Tmp = 0;
  uint8_t u8Command = 0;
  uint8_t u8Command2 = 0;
  static stc_maerklin_292xx_ir_codeset_t* pLastCodeSet = 0;
  int i;
  if (debugMode)
  {
     Serial.println(">> Send:");
     
     Serial.print("  - enAddress: 0x");
     Serial.print(enAddress,HEX),
     Serial.println("");
    
     Serial.print("  - enFunction: 0x");
     Serial.print(enFunction, HEX),
     Serial.println("");
  }

  //
  //  Handle Locos type address A & B
  //
  if ((enAddress == enMaerklin292xxIrAddressA) || (enAddress == enMaerklin292xxIrAddressB))
  {
      u8Repeat = 0;
      codeCache[0] = 20000;
      codeCache[17] = 45000;
      codeCache[18] = 20000;
      u8CommandLen = 8;
      stc_maerklin_292xx_ir_channelset_t* pstcChannelSet = 0;
      stc_maerklin_292xx_ir_codeset_t* pstcCodeSet = 0;

      //
      // Select address
      //
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

      //
      // Check for command
      //
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

      //
      //  Handle toggling
      //
      if (pLastCodeSet != pstcCodeSet)
      {
         bToggle = false;
         pLastCodeSet = pstcCodeSet;
      }

      //
      //  Choose command depending on toggle bit
      //
      if (bToggle)
      {
        u8Command = pstcCodeSet->codes[1] & 0xff;
        u8Command2 = (pstcCodeSet->codes[1] >> 8) & 0xff;
      } else {
        u8Command = pstcCodeSet->codes[0] & 0xff;
        u8Command2 = (pstcCodeSet->codes[0] >> 8) & 0xff;
      }
  } 
  
  //
  //  Handle Locos type address C & D
  //
  else if ((enAddress == enMaerklin292xxIrAddressC) || (enAddress == enMaerklin292xxIrAddressD))
  {
      u8Repeat = 0;
      codeCache[0] = 5400;
      codeCache[17] = 10000;
      codeCache[18] = 5400;
      u8CommandLen = 8;
      u8Command = (((uint8_t)enFunction & 0x7) << 4);
      if (enAddress == enMaerklin292xxIrAddressC)
      {
        u8Command |= 0x05; 
      } else if (enAddress == enMaerklin292xxIrAddressD)
      {
        u8Command |= 0x0a; 
      }
      if (u8Command != u8LastCommand)
      {
        bToggle = false;
      }
      u8LastCommand = u8Command;
      
      if (bToggle)
      {
        u8Command |= 0x80;
      }
  } 

  //
  //  Handle Locos type address G & H
  //
  else if ((enAddress == enMaerklin292xxIrAddressG) || (enAddress == enMaerklin292xxIrAddressH))
  {
      codeCache[0] = 4100;
      codeCache[1] = 550;
      codeCache[30] = 1700;
      u8CommandLen = 15;
      u16Tmp = 0;
      if (enAddress == enMaerklin292xxIrAddressH)
      {
        u16Tmp |= (1 << 6);
      }
      if ((enFunction & 0x70) == 0)
      {
        enFunction = (enFunction & ~0x70) | enMaerklin292xxIrFuncSpeedStop;
      }
      u16Tmp |= ((enFunction & 0x70) >> 1) & 0x38;
      u32UpdateRate = 200;
      if (enFunction & 0x0f)
      {
        if (u8Repeat == 0)
         {
           u8Repeat = 3;
           u32UpdateRate = 20;
         }
      }
      switch(enFunction & 0x0f)
      {
        case enMaerklin292xxIrFuncLight:
           u16Tmp |= 0x4;
           break; 
        case enMaerklin292xxIrFuncSoundHorn:
           u16Tmp |= 0x3;
           break; 
        case enMaerklin292xxIrFuncSoundAnnouncement:
           u16Tmp |= 0x2;
           break; 
        case enMaerklin292xxIrFuncSound1:
           u16Tmp |= 0x1;
           break; 
        default:
           break;
      }
      if (u8Repeat == 0)
      {
        u8Repeat = 10;
      }
      u16Tmp = (u16Tmp << 7) | (~u16Tmp & 0x7f);
      for(i = 0;i < 14;i++)
      {
        if ((u16Tmp & (1 << 13)) != 0) 
        {
          codeCache[i*2 + 2] = 1500;
          codeCache[i*2 + 3] = 550;
        } else {
          codeCache[i*2 + 2] = 550;
          codeCache[i*2 + 3] = 550;
        }
        u16Tmp = u16Tmp << 1;
      }
  }

  //
  //  If command len == 8, handle 2 times 8 bits
  //
  if (u8CommandLen == 8)
  {  
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
    if (enAddress <= enMaerklin292xxIrAddressD)
    {
      delay(200);
    }
    bToggle = !bToggle;
  } else if (u8CommandLen == 15)
  {
    irsend.sendRaw(codeCache,29,38);
  }

  //
  // Saving last states
  //
  au8LastStates[(uint8_t)enAddress] = enFunction;
  enLastAddress = enAddress;
}

/*
 * Set speed
 * 
 * \param enAddress  Address, can be enMaerklin292xxIrAddressA...H
 * 
 * \param speed  can be -3,-2,-1,0,1,2,3
 */
void Maerklin292xxIr_SetSpeed(en_maerklin_292xx_ir_address_t enAddress, int speed)
{
  uint8_t u8Temp;
  if (debugMode)
  {
     Serial.println("SetSpeed:");
     
     Serial.print("  - enAddress: 0x");
     Serial.print(enAddress,HEX),
     Serial.println("");
    
     Serial.print("  - speed: ");
     Serial.print(speed),
     Serial.println("");
  }
  
  if ((speed > 3) || (speed < -3))
  {
    speed = 0; //emergency stop, speed value was wrong
  }
  
  //
  //  Handle Locos type address A, B, C, D
  //     
  if (enAddress <= enMaerklin292xxIrAddressD)
  {
    Maerklin292xxIr_Send(enAddress,enMaerklin292xxIrFuncStop);
    if (speed > 0)
    {
       while(speed != 0)
       {
          speed--;
          Maerklin292xxIr_Send(enAddress,enMaerklin292xxIrFuncForward);
       }
    } else if (speed < 0)
    {
       while(speed != 0)
       {
          speed++;
          Maerklin292xxIr_Send(enAddress,enMaerklin292xxIrFuncBackward);
       }
    } 
  } 
  //
  //  Handle Locos type address > D
  //
  else
  {
    au8LastStates[(uint8_t)enAddress] = 0;
    if (speed == 0)
    {
       u8Temp = enMaerklin292xxIrFuncSpeedStop;
    } else if (speed > 0)
    {
       u8Temp = speed - 1 + (enMaerklin292xxIrFuncSpeedForward1 >> 4);
    }  else if (speed < 0)
    {
       speed = -speed;
       u8Temp = speed - 1 + (enMaerklin292xxIrFuncSpeedBackward1 >> 4);
    }
    if (u8Repeat != 0)
    {
      u8Repeat = 0;
      delay(500);
    }
    Maerklin292xxIr_Send(enAddress,(u8Temp << 4));
  }
}

/*
 * Set speed
 * 
 * \param enAddress  Address, can be enMaerklin292xxIrAddressA...H
 * 
 * \param enFunction  can be any sound or light function
 */
void Maerklin292xxIr_ToggleSoundLight(en_maerklin_292xx_ir_address_t enAddress, en_maerklin_292xx_ir_func_t enFunction)
{
    static uint8_t u8Tmp;
    if (enAddress <= enMaerklin292xxIrAddressD)
    {
       Maerklin292xxIr_Send(enAddress,(uint8_t)enFunction);
    } else
    {
       if (debugMode)
       {
         Serial.println("ToggleSoundLight:");
         
         Serial.print("  - enAddress: 0x");
         Serial.print(enAddress,HEX),
         Serial.println("");
  
         Serial.print("  - LastState: 0x");
         Serial.print(au8LastStates[(uint8_t)enAddress],HEX),
         Serial.println("");
       }

       au8LastStates[(uint8_t)enAddress] = au8LastStates[(uint8_t)enAddress] & 0xf0;

       if (debugMode)
       {
         Serial.print("  - LastState(edited): 0x");
         Serial.print(au8LastStates[(uint8_t)enAddress],HEX),
         Serial.println("");
       }

       u8Tmp = au8LastStates[(uint8_t)enAddress];
       u8Tmp |= ((uint8_t)enFunction & 0xf);

       if (debugMode)
       {
         Serial.print("  - Request Command: 0x");
         Serial.print(enFunction,HEX),
         Serial.println("");
         
         Serial.print("  - New Command: 0x");
         Serial.print(u8Tmp,HEX),
         Serial.println("");
       }
       
       if (u8Repeat != 0)
       {
          u8Repeat = 0;
          if (debugMode)
          {
            Serial.println("  *** DELAYING 10ms ***");
          }
          delay(10);
       }
       Maerklin292xxIr_Send(enAddress,u8Tmp);
    }
}


/*
 * Update IR sending in loop
 */
void Maerklin292xxIr_Update(void)
{
    //
    // Sending repeated commands is only supported by locomotives with addresses > D
    //
    if ((u8Repeat > 0) && (enLastAddress > enMaerklin292xxIrAddressD) && ((millis() - u32LastUpdate) > u32UpdateRate))
    {
      u32LastUpdate = millis();
      Maerklin292xxIr_Send(enLastAddress,au8LastStates[(uint8_t)enLastAddress]);
      u8Repeat--;

      //
      // was last repeated command?
      //
      if (u8Repeat == 0)
      {
        //
        // if was a sound or light function, repeat 2 times the current driving command
        //
        if (au8LastStates[(uint8_t)enLastAddress] & 0x0f)
        {
           u8Repeat = 2;
           au8LastStates[(uint8_t)enLastAddress] = au8LastStates[(uint8_t)enLastAddress] & 0xf0;
        }
      }
    }
}

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
