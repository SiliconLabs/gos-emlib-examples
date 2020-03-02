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
#include "em_opamp.h"
#include "em_cmu.h"

#define APPLICATION_START_LINE    "\r\n\r\nEMLIB Unity Gain ADC Example!!"
#define ADC_MEASUREMENT_RATE_MS   1000

// Geck OS ADC Configuration Struct
static const gos_adc_config_t adc_config =
{
  .resolution = 12
};


/*************************************************************************************************/
void gos_app_init(void)
{
  GOS_LOG(APPLICATION_START_LINE);

  //Enable the OpAmp clock source.  The OpAmp registers are located in the VDAC peripheral
  CMU_ClockEnable(cmuClock_VDAC0, true);  

  // Initialize the OpAmp
  OPAMP_Init_TypeDef opamp_init =  OPA_INIT_UNITY_GAIN;   // Default for Unity Gain configuration
  opamp_init.posSel  = opaPosSelAPORT4XCH15;              // Choose opamp positive input to come from PE15 (EXP14)
  opamp_init.outMode = opaOutModeAPORT4YCH14;             // Route opamp output to PE14

  // No APORT signal is explicitly routed to the negative input of the opamp
  // because the default OPA_INIT_UNITY_GAIN macro already takes care of
  // routing the output back to the negative input

  // Enable OPA2
  OPAMP_Enable(VDAC0, OPA2, &opamp_init);

  // Initialize the ADC
  gos_adc_init(GOS_GPIO_0, &adc_config);   // GOS_GPIO_0 = PE14 = EXP12

  // Register a periodic event to perform measurements
  gos_event_register_periodic(perform_adc_measurement, 0, ADC_MEASUREMENT_RATE_MS, 0);
}


/*************************************************************************************************/
static void perform_adc_measurement(void *arg)
{
	uint16_t adc_raw = 0;
	uint32_t millivolts;

  //Perform the ADC measurement
	gos_adc_sample(GOS_GPIO_0, &adc_raw, GOS_ADC_SAMPLE_TYPE_RAW);

  // Conver to millivolts
  millivolts = (adc_raw * 2500) / 4096;
  
  // Print the result to the console
  GOS_LOG("mV = %d", millivolts);
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
    // This API is optional.
}
