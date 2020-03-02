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
#include "em_csen.h"
#include "em_cmu.h"

#define APPLICATION_START_LINE     "\r\n\r\nCap Sense Example!!"
#define CSEN_MEASUREMENT_RATE_MS   500
#define TOUCH_THRESHOLD            40000

static bool touch_state = false;


/*************************************************************************************************/
void gos_app_init(void)
{

  GOS_LOG(APPLICATION_START_LINE);
    
  // Enable the CSEN peripheral clock
  CMU_ClockEnable(cmuClock_CSEN_HF, true);
  
  // Initialize the CSEN peripheral
  CSEN_Init_TypeDef csenInit = CSEN_INIT_DEFAULT;    // Use the default settings
  CSEN_Init(CSEN, &csenInit);

  // Select the input pin and initialize the conversion mode
  CSEN_InitMode_TypeDef csenInitMode = CSEN_INITMODE_DEFAULT;   // Use the default conversion mode
  csenInitMode.singleSel = csenSingleSelAPORT3YCH5;             // Select PE5 as the input (hurricane board - https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/hurricane/)
  CSEN_InitMode(CSEN, &csenInitMode);

  // Enable the CSEN peripheral
  CSEN_Enable(CSEN); 
  
  // Register a periodic event to poll the touch pad
  gos_event_register_periodic(poll_csen_handler, 0, CSEN_MEASUREMENT_RATE_MS, 0);

  GOS_LOG("Place your finger on/off the touch pad");
}



/*************************************************************************************************/
static void poll_csen_handler(void *arg)
{ 
  uint32_t touch_value=0;

	// Start a measurement
	CSEN_Start(CSEN);
	
	// Wait for measurement to complete
	while(CSEN_IsBusy(CSEN) == true)
	{ gos_wait_milliseconds(1);        
	}
	
	// Read the measurement result
	touch_value = CSEN_DataGet(CSEN);
  
  // Output the result
  if((touch_value > TOUCH_THRESHOLD) && (touch_state == false))
  { GOS_LOG("%d - Touch Detected", touch_value);
    touch_state = true;
  }  
  else if((touch_value < TOUCH_THRESHOLD) && (touch_state == true))
  { GOS_LOG("%d - Touch Removed", touch_value);
    touch_state = false;
  }
  else
    GOS_LOG("%d", touch_value);
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