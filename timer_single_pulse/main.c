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
#include "em_timer.h"
#include "em_cmu.h"

#define APPLICATION_START_LINE     "\r\n\r\nEMLIB Hardware Triggered Pulse Example!!"

// Configure the trigger source of the pulse
#define USE_PWM_SOURCE             1                 //1 = uses PWM as the source; 0 = use button press as the source

//The first edge of the pulse occurs this many us after the input edge
#define DELAY_TIME_US              100               

//The width of the pulse
#define PULSE_WIDTH_US             150               

// Button Configurations
#define BUTTON_DEBOUNCE_TIME_MS    (50)   
#define BUTTON_CLICK_TIME_MS       (500)  
#define BUTTON_PRESS_TIME_MS       (1000) 


/*************************************************************************************************/
void gos_app_init(void)
{
  GOS_LOG(APPLICATION_START_LINE);

  const gos_button_config_t config =
  {
    .active_level         = PLATFORM_BUTTON_ACTIVE_STATE,
    .debounce             = BUTTON_DEBOUNCE_TIME_MS,
    .click_time           = BUTTON_CLICK_TIME_MS,
    .press_time           = BUTTON_PRESS_TIME_MS,
    .event_handler.press  = NULL,
    .event_handler.click  = button_clicked_event_handler,
    .event_handler.toggle = NULL,
    .execution_context    = GOS_BUTTON_CONTEXT_DEFAULT
  };

  // Configure the buttons
  gos_button_init(PLATFORM_BUTTON1, &config, (void*)1);
  gos_button_init(PLATFORM_BUTTON2, &config, (void*)2);

  // Initialize the timer
  initialize_timer();
    
  // Configure the timer to generate a pulse
  configure_pulse(DELAY_TIME_US, PULSE_WIDTH_US);

  // Configure the GPIO used to output the pulse created by the timer
  gos_gpio_init(GOS_GPIO_2, GOS_GPIO_OUTPUT_PUSH_PULL, false);    //GOS_GPIO_2 = PA0 = EXP4

  // Configure the GPIO used as the input to trigger the timer
  gos_gpio_init(GOS_GPIO_3, GOS_GPIO_INPUT_HIGH_IMPEDANCE, false);   //GOS_GPIO_3 = PA1 = EXP6

  // GOS_GPIO_6 (PA4) is used as the source to GOS_GPIO_3.  It is toggled either by a button press or PWM 
  // depending on the value of USE_PWM_SOURCE.  Wire GOS_PIO_6 to GOS_GPIO_3 to allow the button or PWM 
  // to trigger the pulse.  In a "real" application the source for GOS_GPIO_3 will be a different piece or 
  // hardware such as a zero-cross detector

#if (USE_PWM_SOURCE == 0)
  gos_gpio_init(GOS_GPIO_6, GOS_GPIO_OUTPUT_PUSH_PULL, false);    //GOS_GPIO_6 = PA4 = EXP7
#else

  // For this to work the PWM period must be sufficiently long enough to allow the configured timer to complete on both the
  // rising and falling edge...So if DELAY_TIME_US is 100us and PULSE_WIDTH_US is 150 the total timer cycle is 250us per edge
  // so the PWM period should be larger than 250us x 2 = 500us (2000 Hz)
  gos_pwm_update(GOS_GPIO_6, 5000, 1000);

  // If using PWM as the source then a button press will perform a NVM save...GOS_GPIO_1 will be toggled during
  // the NVM save to allow you to trigger a scope
  gos_gpio_init(GOS_GPIO_1, GOS_GPIO_OUTPUT_PUSH_PULL, false);    //GOS_GPIO_1 = PE15 = EXP14

#endif

}


/*************************************************************************************************/
static void initialize_timer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize the configuration struct 
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.oneShot = true;
  timerInit.mode = timerModeUp;
  timerInit.prescale = timerPrescale1;
  timerInit.riseAction = timerInputActionReloadStart;        // start timer on rising edge of CC0 input
  timerInit.fallAction = timerInputActionReloadStart;        // start timer on falling edge of CC0 input

  // Initialize the timer
  TIMER_Init(TIMER0, &timerInit);

  // Initialize Compare Capture Channel 1 as the pulse output
  TIMER_InitCC_TypeDef timerCCInit_1 = TIMER_INITCC_DEFAULT;
  timerCCInit_1.cmoa = timerOutputActionToggle;                   // Toggle GPIO output on compare match...this is the first edge of the pulse
  timerCCInit_1.cofoa = timerOutputActionToggle;                  // Toggle GPIO output on counter overflow...this is the second edge of the pulse
  timerCCInit_1.coist = false;                                    // Set initial value of the output low ... to invert the pulse, set this to true
  timerCCInit_1.mode = timerCCModeCompare;
  TIMER_InitCC(TIMER0, 1, &timerCCInit_1);

  // Map Compare Capture Channel 1 to GPIO
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC1LOC_LOC7;      // TIM0_CC1 #7 is PA0 (GOS_GPIO_2)
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;


  // Initialize Compare Capture Channel 0 as the trigger input
  TIMER_InitCC_TypeDef timerCCInit_0 = TIMER_INITCC_DEFAULT;
  timerCCInit_0.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit_0);

  // Map Compare Capture Channel 0 to GPIO
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC7;      // TIM0_CC0 #7 is PA1 - GOS_GPIO_3 - EXP6
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
}


/*************************************************************************************************/
static void configure_pulse(uint32_t delay_us, uint32_t pulse_width_us)
{
  uint32_t rising_edge_count = delay_us * (CMU_ClockFreqGet( cmuClock_HFPER )/1000000);
  uint32_t falling_edge_count = (delay_us + pulse_width_us) * (CMU_ClockFreqGet( cmuClock_HFPER )/1000000);

  // Note that if you want to update these value while the timer is running you should instead write to the Top buffer and CC buffer

  // The first edge of the pulse is created by the compare match
  TIMER_CompareSet(TIMER0, 1, rising_edge_count);

  // The second edge of the pulse is created by counter overflow
  TIMER_TopSet(TIMER0, falling_edge_count);
}


/*************************************************************************************************/
bool toggle = false;

static void button_clicked_event_handler(void *arg)
{
  uint32_t button_number = (uint32_t)arg;
  GOS_LOG("Button%d: click", button_number);


  toggle = !toggle;

  // if USE_PWM_SOURCE == 0 then we simply toggle the GPIO that is used as a trigger.  Otherwise we perform a NVM save
  // operation to demonstrate that the pulse output is not interrupted by the NVM save process.
 
#if (USE_PWM_SOURCE == 0)
  gos_gpio_set(GOS_GPIO_6, toggle);   // toggle the source GPIO
#else
  GOS_LOG("Saving NVM");
  gos_cmd_issue ("set ethernet.dhcp.enabled %d", toggle == true ? 1 : 0);   //change a setting to force the NVM save to take a long time
  gos_gpio_set(GOS_GPIO_1, true);      // toggle GPIO to allow triggering a scope
  gos_cmd_issue ("save");
  gos_gpio_set(GOS_GPIO_1, false);     // return GPIO to allow triggering a scope
  GOS_LOG("Done");
#endif

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
