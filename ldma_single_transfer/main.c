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
#include "em_ldma.h"

#define APPLICATION_START_LINE      "\r\n\r\nLDMA Single Transfer Example!!"
#define BUFFER_SIZE                 127
#define TRANSFER_SIZE               (BUFFER_SIZE - 1)
#define PRINT_HALF_WORDS_PER_LINE   16            

// The memory buffers used for the transfer
uint16_t srcBuffer[BUFFER_SIZE];    // source 
uint16_t dstBuffer[BUFFER_SIZE];    // destination

// dma channel allocated from Gecko OS
gos_dma_channel_t dma_ch;           


/*************************************************************************************************/
void gos_app_init(void)
{
  uint32_t i;
  
  // Initialize the LDMA configuration structures
  const LDMA_TransferCfg_t transferCfg = LDMA_TRANSFER_CFG_MEMORY();
  const LDMA_Descriptor_t desc = LDMA_DESCRIPTOR_SINGLE_M2M_HALF(srcBuffer, dstBuffer, TRANSFER_SIZE);

  GOS_LOG(APPLICATION_START_LINE);

  // Initialize the buffers for memory transfer 
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    srcBuffer[i] = i;
    dstBuffer[i] = 0;
  }
  
  // Enable the ability to issue events from the IRQ context
  gos_event_enable_irq_events(1);

  // Allocated a DMA channel 
  gos_dma_alloc(dma_irq_callback, NULL, &dma_ch);
  
  GOS_LOG("Requesting DMA transfer on channel %d", dma_ch);

  // Start the memory transfer 
  LDMA_StartTransfer(dma_ch, &transferCfg, &desc);  

}


/*************************************************************************************************/
static void dma_irq_callback( void *arg )
{ 
  // The DMA callback executes in the IRQ context therefore it is important to keep this code
  // at a minimum.  In this case we only register an event to do the work.  The event handler will
  // execute in the applilcation context so it can perform actions such as GOS_LOG().  
  //
  // NOTE: The application must enable the ability to issue events from the IRQ context.  In this 
  // example this is performed by calling gos_event_enable_irq_events() in gos_app_init()
  
  gos_event_issue(dma_complete_handler, NULL, GOS_EVENT_FLAG_NONE);
}


/*************************************************************************************************/
static void dma_complete_handler(void * arg)
{ 
  uint32_t i;
  bool match = true;

	// Free the DMA channel since this is a one-time transfer 
	gos_dma_free(dma_ch);

  // Disable the ability to issue events from the IRQ context
  gos_event_disable_irq_events();

	// Compare the buffers  
  for(i = 0; i < TRANSFER_SIZE; i++)	
  { if(srcBuffer[i] != dstBuffer[i])
      match = false;
  }
  
  // Print the results
  if(match == true)
  	GOS_LOG("\r\nTransfer Successful!");
  else
  	GOS_LOG("\r\nTransfer Failed!");

  print_buffer(dstBuffer, TRANSFER_SIZE);
}



/*************************************************************************************************/
static void print_buffer(uint16_t *buffer, uint32_t length)
{ uint32_t i;
  char print_message[PRINT_HALF_WORDS_PER_LINE * 5 + 1];   // each half word will be 5 characters (4 digits + 1 space)
	
	print_message[0] = 0;    //start with an empty string
	
	// Loop through the buffer and print each line
	for(i = 0; i < length; i++)
	{ 
    sprintf(&print_message[strlen(print_message)], "%4x ", buffer[i]);

    // If we are ready then print a line and reset the string
    if(((i + 1) % PRINT_HALF_WORDS_PER_LINE) == 0)   		
    { GOS_LOG(print_message);
    	print_message[0] = 0;
    }	
	}
	
	// Print the rest of the data
  if(strlen(print_message) > 0)
  { GOS_LOG(print_message);
  }
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