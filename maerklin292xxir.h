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

#if !defined(__MAERKLIN292XXIR_H__)
#define __MAERKLIN292XXIR_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *******************************************************************************
 ** \defgroup Maerklin292xxIrGroup Maerklin292xx IR protocol using IRSend
 **
 ** Provided functions of Maerklin292xxIr:
 **
 **
 *******************************************************************************
 */

//@{

/**
 *******************************************************************************
** \page maerklin292xxir_module_includes Required includes in main application
** \brief Following includes are required
** @code
** #include "maerklin292xxir.h"
** @endcode
**
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** (Global) Include files
 *******************************************************************************
 */

#include <stdint.h>

/**
 *******************************************************************************
 ** Global pre-processor symbols/macros ('#define') 
 *******************************************************************************
 */

//#define MAERKLIN292XXIR_IR_PIN 4 //Moved to appconfig.h, INITIAL_GPIO_IR, use http://maerklin292xx-gateway.local/config/ to configure GPIO pin usage
/**
 *******************************************************************************
 ** Global type definitions ('typedef') 
 *******************************************************************************
 */

typedef enum en_maerklin_292xx_ir_address
{
  enMaerklin292xxIrAddressA = 0x1,
  enMaerklin292xxIrAddressB = 0x2,
  enMaerklin292xxIrAddressC = 0x3,
  enMaerklin292xxIrAddressD = 0x4,
  enMaerklin292xxIrAddressG = 0x7,
  enMaerklin292xxIrAddressH = 0x8,
  enMaerklin292xxIrAddressI = 0x9,
  enMaerklin292xxIrAddressJ = 0xa,
} en_maerklin_292xx_ir_address_t;


typedef enum en_maerklin_292xx_ir_func
{
  enMaerklin292xxIrFuncStop = 0,
  enMaerklin292xxIrFuncBackward = 1,
  enMaerklin292xxIrFuncForward = 2,
  enMaerklin292xxIrFuncSound1 = 3,
  enMaerklin292xxIrFuncSoundDoorClose = 3,
  enMaerklin292xxIrFuncSoundCoupler = 3,
  enMaerklin292xxIrFuncSound2 = 4,
  enMaerklin292xxIrFuncSoundHorn = 4,
  enMaerklin292xxIrFuncSound3 = 5,
  enMaerklin292xxIrFuncSoundMotor = 5,
  enMaerklin292xxIrFuncSoundAnnouncement = 5,
  enMaerklin292xxIrFuncLight = 6,
  enMaerklin292xxIrFuncSpeedForward1 = 0x10,
  enMaerklin292xxIrFuncSpeedForward2 = 0x20,
  enMaerklin292xxIrFuncSpeedForward3 = 0x30,
  enMaerklin292xxIrFuncSpeedBackward1 = 0x40,
  enMaerklin292xxIrFuncSpeedBackward2 = 0x50,
  enMaerklin292xxIrFuncSpeedBackward3 = 0x60,
  enMaerklin292xxIrFuncSpeedStop = 0x70,
} en_maerklin_292xx_ir_func_t;

typedef uint16_t maerklin_292xx_ir_code_t;

typedef struct stc_maerklin_292xx_ir_codeset
{
  maerklin_292xx_ir_code_t codes[2];
} stc_maerklin_292xx_ir_codeset_t;

typedef struct stc_maerklin_292xx_ir_channelset
{
  stc_maerklin_292xx_ir_codeset_t stop;
  stc_maerklin_292xx_ir_codeset_t backward;
  stc_maerklin_292xx_ir_codeset_t forward;
  stc_maerklin_292xx_ir_codeset_t light;
  union {
    stc_maerklin_292xx_ir_codeset_t sound1;
    stc_maerklin_292xx_ir_codeset_t soundDoorClose;
    stc_maerklin_292xx_ir_codeset_t soundCoupler;
  };
  union {
    stc_maerklin_292xx_ir_codeset_t sound2;
    stc_maerklin_292xx_ir_codeset_t soundHorn;
  };
  union {  
    stc_maerklin_292xx_ir_codeset_t sound3;
    stc_maerklin_292xx_ir_codeset_t soundMotor;
    stc_maerklin_292xx_ir_codeset_t soundAnnouncement;
  };
} stc_maerklin_292xx_ir_channelset_t;

/**
 *******************************************************************************
 ** Global variable declarations ('extern', definition in C source)
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** Global function prototypes ('extern', definition in C source) 
 *******************************************************************************
 */

void Maerklin292xxIr_Init(void);
void Maerklin292xxIr_Send(en_maerklin_292xx_ir_address_t enAddress, uint8_t enFunction);
void Maerklin292xxIr_SetSpeed(en_maerklin_292xx_ir_address_t enAddress, int speed);
void Maerklin292xxIr_ToggleSoundLight(en_maerklin_292xx_ir_address_t enAddress, en_maerklin_292xx_ir_func_t enFunction);
void Maerklin292xxIr_Update(void);

//@} // Maerklin292xxIrGroup

#ifdef __cplusplus
}
#endif

#endif /* __MAERKLIN292XXIR_H__ */

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
