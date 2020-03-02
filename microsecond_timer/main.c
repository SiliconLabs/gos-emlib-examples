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

#define APPLICATION_START_LINE   "\r\n\r\nEMLIB Microsecond Timer Example!!"

// The delay time of the example in microseconds
#define DELAY_TIME_US            100            

// Button Configurations
#define BUTTON_DEBOUNCE_TIME_MS  (50)   
#define BUTTON_CLICK_TIME_MS     (500)  
#define BUTTON_PRESS_TIME_MS     (1000) 


/*************************************************************************************************/
void gos_app_init(void)
{
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

  GOS_LOG(APPLICATION_START_LINE);

  // Initialize the timer
  initialize_timer();

  // Enable the timer interrupt and configure the handler
  NVIC_EnableIRQ(TIMER1_IRQn);
  gos_system_set_irq_callback(TIMER1_IRQn, TIMER1_IRQHandler);   // Set the timer interrupt handler

  // Configure the GPIO output 
  gos_gpio_init(GOS_GPIO_7, GOS_GPIO_OUTPUT_PUSH_PULL, false);   // GOS_GPIO_7 = PA5 = EXP9

  // Configure the buttons
  gos_button_init(PLATFORM_BUTTON1, &config, (void*)1);
  gos_button_init(PLATFORM_BUTTON2, &config, (void*)2);
}


/*************************************************************************************************/
static void initialize_timer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Initialize the configuration struct for one-shot countdown mode
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.oneShot = true;                    
  timerInit.mode = timerModeDown;    
  timerInit.prescale = timerPrescale1;
  
  // Initialie the timer
  TIMER_Init(TIMER1, &timerInit);
}


/*************************************************************************************************/
static void button_clicked_event_handler(void *arg)
{
	uint32_t button_number = (uint32_t)arg;
  
  GOS_LOG("Button%d: click", button_number);

	// Calculate the timer value needed to create the delay
	uint16_t timer_cycle_count = DELAY_TIME_US * (CMU_ClockFreqGet( cmuClock_HFPER )/1000000);

	// Stop the time in case it is already running
  TIMER_Enable(TIMER1, false);

  // Set the timer value
  TIMER_CounterSet(TIMER1, timer_cycle_count);

  if(button_number == 1)  //Button 1 (PB0) will use interrupts
  {
    // Enable TIMER1 underflow interrupts
    TIMER_IntClear(TIMER1, TIMER_IEN_UF);
    TIMER_IntEnable(TIMER1, TIMER_IEN_UF);

    // Assert the GPIO
    gos_gpio_set(GOS_GPIO_7, true);

    // Enable the TIMER
    TIMER_Enable(TIMER1, true);
  }
  else if(button_number == 2)    //Button 2 (PB1) will not use interrupts
  {
    // Disable TIMER1 interrupts since this method does not use interrupts
    TIMER_IntDisable(TIMER1, TIMER_IEN_UF);
    TIMER_IntClear(TIMER1, TIMER_IEN_UF);

    //Assert the GPIO
    gos_gpio_set(GOS_GPIO_7, true);

    // Enable the TIMER
    TIMER_Enable(TIMER1, true);

    // Wait for timer underflow
    while ((TIMER_IntGet(TIMER1) & TIMER_IF_UF) == 0){}

    // Deassert the GPIO
    gos_gpio_set(GOS_GPIO_7, false);
  }
}


/*************************************************************************************************/
void TIMER1_IRQHandler(void)
{ uint32_t flags = TIMER_IntGet(TIMER1);
  
  // Clear the interrrupt
  TIMER_IntClear(TIMER1, flags);

  // Deassert the GPIO if timer underflow
  if(flags & TIMER_IF_UF)
    gos_gpio_set(GOS_GPIO_7, false);  
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
