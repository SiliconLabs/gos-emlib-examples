/*******************************************************************************
 * # License
 * Copyright 2020 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "gos.h"
#include "em_system.h"

#define APPLICATION_START_LINE "\r\n\r\nEMLIB Part Info Example!!"


/*************************************************************************************************/
void gos_app_init(void)
{

  GOS_LOG(APPLICATION_START_LINE);
    
  GOS_LOG("Part Number:    %d", (int)SYSTEM_GetPartNumber());
  GOS_LOG("Part Family:    %d", (int)SYSTEM_GetFamily());
  GOS_LOG("Part Revision:  %d", (int)SYSTEM_GetProdRev());

}

/*************************************************************************************************/
void gos_app_deinit(void)
{
    // This API is called just before the application finishes.
    // Anything initialized or allocated by the app should be cleaned here.
    // This API is optional.
}


/*************************************************************************************************/
void gos_app_idle(void)
{
    // This is called when the event thread has no more pending events.

}