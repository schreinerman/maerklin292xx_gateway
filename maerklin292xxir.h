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

#define MAERKLIN292XXIR_IR_PIN 12
/**
 *******************************************************************************
 ** Global type definitions ('typedef') 
 *******************************************************************************
 */

typedef enum en_maerklin_292xx_ir_address
{
  enMaerklin292xxIrAddressC = 0x5,
  enMaerklin292xxIrAddressD = 0xa,
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
} en_maerklin_292xx_ir_func_t;

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
void Maerklin292xxIr_Send(en_maerklin_292xx_ir_address_t enAddress, en_maerklin_292xx_ir_func_t enFunction);

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
