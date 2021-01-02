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
 **\file irgatewaywebserver.c
 **
 ** WebServer used as IR gateway
 ** A detailed description is available at
 ** @link IrGatewayWebServerGroup file description @endlink
 **
 ** History:
 ** - 2021-1-2  1.00  Manuel Schreiner
 *******************************************************************************
 */

#if !defined(__IRGATEWAYWEBSERVER_H__)
#define __IRGATEWAYWEBSERVER_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 *******************************************************************************
 ** \defgroup IrGatewayWebServerGroup WebServer used as IR gateway
 **
 ** Provided functions of IrGatewayWebServer:
 **
 **
 *******************************************************************************
 */

//@{

/**
 *******************************************************************************
** \page irgatewaywebserver_module_includes Required includes in main application
** \brief Following includes are required
** @code
** #include "irgatewaywebserver.h"
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

/**
 *******************************************************************************
 ** Global type definitions ('typedef') 
 *******************************************************************************
 */



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

void IrGatewayWebServer_Init(void);
void IrGatewayWebServer_Update(void);

//@} // IrGatewayWebServerGroup

#ifdef __cplusplus
}
#endif

#endif /* __IRGATEWAYWEBSERVER_H__ */

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */
