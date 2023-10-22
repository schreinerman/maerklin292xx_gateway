/**
 *******************************************************************************
 ** Created by Manuel Schreiner
 **
 ** Copyright © 2023 Manuel Schreiner. All rights reserved.
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
 **\file mdnsclientlist.cpp
 **
 ** Listing other MDNS Clients
 ** A detailed description is available at
 ** @link MdnsClientListGroup file description @endlink
 **
 ** History:
 ** - 2023-10-22  1.00  Manuel Schreiner
 *******************************************************************************
 */

#if !defined(__MDNSCLIENTLIST_H__)
#define __MDNSCLIENTLIST_H__

/**
 *******************************************************************************
 ** \defgroup MdnsClientListGroup Listing other MDNS Clients
 **
 ** Provided functions of MdnsClientList:
 **
 **
 *******************************************************************************
 */

//@{

/**
 *******************************************************************************
** \page mdnsclientlist_module_includes Required includes in main application
** \brief Following includes are required
** @code
** #include "mdnsclientlist.h"
** @endcode
**
 *******************************************************************************
 */

/**
 *******************************************************************************
 ** (Global) Include files
 *******************************************************************************
 */

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

typedef struct stc_mdnsclientlist_item
{
    char ipAddress[18];
} stc_mdnsclientlist_item_t;

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


void MdnsClientList_Init(const char* pstrService);
void MdnsClientList_Update(void);

//@} // MdnsClientListGroup

#endif /* __MDNSCLIENTLIST_H__ */

/**
 *******************************************************************************
 ** EOF (not truncated)
 *******************************************************************************
 */

