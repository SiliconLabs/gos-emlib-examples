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
#include "em_cmu.h"
#include "em_timer.h"

#define APPLICATION_START_LINE "\r\n\r\nPWM with Delay Time Insertion Example"

#define PWM_FREQUENCY_HZ 1000


/*************************************************************************************************/
void gos_app_init(void)
{
  GOS_LOG(APPLICATION_START_LINE);

  // Configure the PWM channels and outputs
  configure_pwm_channels();
  
  // Start the PWM output
  start_pwm();
}


/*************************************************************************************************/
static void configure_pwm_channels()
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

/* CHANNEL    GOS_GPIO_#    TIMER# | CHAN  | LOC      PIN     EXP
 * ----------------------------------------------------------------
 *  1           2           TIMER0 | CC0   |  0       PA0     EXP4
 *              5           TIMER0 | CDTI0 |  0       PA3     EXP10
 * ----------------------------------------------------------------
 *  2           3           TIMER0 | CC1   |  0       PA1     EXP6
 *              6           TIMER0 | CDTI1 |  0       PA4     EXP7
 * ----------------------------------------------------------------
 *  3           4           TIMER0 | CC2   |  0       PA2     EXP8
 *              7           TIMER0 | CDTI2 |  0       PA5     EXP9
 */

/*#####  CHANNEL 1  #####*/

  gos_gpio_init(GOS_GPIO_2, GOS_GPIO_OUTPUT_PUSH_PULL, false);   
  gos_gpio_init(GOS_GPIO_5, GOS_GPIO_OUTPUT_PUSH_PULL, false);   
  
  // Connect GOS_GPIO_2 to TIMER0 CC0 LOC #0
  TIMER0->ROUTEPEN |= 0x001;          
  TIMER0->ROUTELOC0 &= ~0x3F;         

  // Connect GOS_GPIO_5 to TIMER0 CDTI0 LOC #0
  TIMER0->ROUTEPEN |= 0x100;
  TIMER0->ROUTELOC2 &= ~0x3F;


/*#####  CHANNEL 2  #####*/

  gos_gpio_init(GOS_GPIO_3, GOS_GPIO_OUTPUT_PUSH_PULL, false);   
  gos_gpio_init(GOS_GPIO_6, GOS_GPIO_OUTPUT_PUSH_PULL, false);   

  // Connect GOS_GPIO_3 to TIMER0 CC1 LOC #0
  TIMER0->ROUTEPEN |= 0x002;          
  TIMER0->ROUTELOC0 &= ~0x00003F00;   

  // Connect GOS_GPIO_6 to TIMER0 CDTI1 LOC #0
  TIMER0->ROUTEPEN |= 0x200;          
  TIMER0->ROUTELOC2 &= ~0x3F00;       


/*#####  CHANNEL 3  #####*/

  gos_gpio_init(GOS_GPIO_4, GOS_GPIO_OUTPUT_PUSH_PULL, false);   
  gos_gpio_init(GOS_GPIO_7, GOS_GPIO_OUTPUT_PUSH_PULL, false);   

  // Connect GOS_GPIO_4 to TIMER0 CC2 LOC #0
  TIMER0->ROUTEPEN |= 0x004;          
  TIMER0->ROUTELOC0 &= ~0x003F0000;   

  // Connect GOS_GPIO_7 to TIMER0 CDTI2 LOC #0
  TIMER0->ROUTEPEN |= 0x400;         
  TIMER0->ROUTELOC2 &= ~0x3F0000;  
}


/*************************************************************************************************/
static void start_pwm()
{
  GOS_LOG("Starting PWM");
  configure_pwm_timer(TIMER0, 0, 2500, PWM_FREQUENCY_HZ);
  configure_pwm_timer(TIMER0, 1, 5000, PWM_FREQUENCY_HZ);
  configure_pwm_timer(TIMER0, 2, 7500, PWM_FREQUENCY_HZ);
}


/*************************************************************************************************/
static void configure_pwm_timer(TIMER_TypeDef *timer, uint8_t cc_channel, uint32_t duty_cycle, uint32_t frequency)
{

  // Note: the prescaler is hardcoded...Change this value according to you desired PWM frequency...or alternatively this can be calculated
  const TIMER_Prescale_TypeDef prescaler = timerPrescale1;
  const uint32_t timer_max_count = CMU_ClockFreqGet( cmuClock_HFPER ) / ((0x1<<prescaler) * frequency);
 
  // Initialize the Compare Capture configuration struct
  TIMER_InitCC_TypeDef timerCCInit =
  {
   .eventCtrl = timerEventEveryEdge,     // Event on every capture.
   .edge      = timerEdgeBoth,           // Input capture edge on rising edge.
   .prsSel    = timerPRSSELCh0,          // Not used by default, select PRS channel 0.
   .cufoa     = timerOutputActionNone,   // No action on underflow.
   .cofoa     = timerOutputActionNone,   // No action on overflow.
   .cmoa      = timerOutputActionToggle, // No action on match.
   .mode      = timerCCModePWM,          // Select PWM mode. 
   .filter    = false,                   // Disable filter. 
   .prsInput  = false,                   // Disable PRS input 
   .coist     = false,                   // Clear output when counter disabled. 
   .outInvert = false,                   // Do not invert output. 
  };

  // Configure CC channel 
  TIMER_InitCC(timer, cc_channel, &timerCCInit);

  // Set Top Value 
  TIMER_TopSet(timer, timer_max_count - 1);

  // Set compare value 
  TIMER_CompareBufSet(timer, cc_channel, (uint32_t)( (uint64_t)timer_max_count * duty_cycle / 10000 ) - 1 );

  // Initialize the Delay Time Insertion (DTI) configuration struct
  TIMER_InitDTI_TypeDef timerDTIInit = TIMER_INITDTI_DEFAULT;
  timerDTIInit.riseTime = 1000;      // use dummy values for now
  timerDTIInit.fallTime = 1000;
  timerDTIInit.outputsEnableMask = TIMER_DTOGEN_DTOGCC0EN | TIMER_DTOGEN_DTOGCDTI0EN | TIMER_DTOGEN_DTOGCC1EN | TIMER_DTOGEN_DTOGCDTI1EN | TIMER_DTOGEN_DTOGCC2EN | TIMER_DTOGEN_DTOGCDTI2EN;

  // Initialize the DTI
  TIMER_InitDTI(timer, &timerDTIInit);

  // Initialize the timer configuration struct
  TIMER_Init_TypeDef timerInit =
  {
   .enable     = true,                  // Enable timer when init complete. 
   .debugRun   = true,                  // Enable counter during debug halt.
   .prescale   = prescaler,             // Select prescaler 
   .clkSel     = timerClkSelHFPerClk,   // Select HFPER clock.
   .fallAction = timerInputActionNone,  // No action on falling input edge.
   .riseAction = timerInputActionNone,  // No action on rising input edge. 
   .mode       = timerModeUp,           // Up-counting. 
   .dmaClrAct  = false,                 // Do not clear DMA requests when DMA channel is active.
   .quadModeX4 = false,                 // Select X2 quadrature decode mode (if used).
   .oneShot    = false,                 // Disable one shot.
   .sync       = false,                 // Not started/stopped/reloaded by other timers.
  };

  // Configure timer
  TIMER_Init(timer, &timerInit);
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
