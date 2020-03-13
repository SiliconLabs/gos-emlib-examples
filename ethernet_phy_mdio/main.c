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
#include "em_eth.h"

#define APPLICATION_START_LINE       "\r\n\r\nEthernet PHY MDIO Register Access Example"

#define DEMO_SETTINGS_MAGIC          1         // Magic number used to load app settings

/*  Note: This example uses a hardcoded PHY address PHY_ADDRESS.  It may be required to modify this
          value for the given hardware
*/
        
#define PHY_ADDRESS                  0                   


/* Macros used to form the commands to interface to the PHY  */
#define PHYMNGMNT_OPERATION(op)      (op << _ETH_PHYMNGMNT_OPERATION_SHIFT)
#define PHYMNGMNT_CLAUSE_22          ETH_PHYMNGMNT_WRITE1
#define PHYMNGMNT_MUST10(must)       (must << _ETH_PHYMNGMNT_WRITE10_SHIFT)
#define PHYMNGMNT_PHYADDR(addr)      (addr << _ETH_PHYMNGMNT_PHYADDR_SHIFT)
#define PHYMNGMNT_REGADDR(reg)       (reg << _ETH_PHYMNGMNT_REGADDR_SHIFT)
#define PHYMNGMNT_DATA(data)         (data << _ETH_PHYMNGMNT_PHYRWDATA_SHIFT)


/*  Custom commands to read and write the PHY registers  */
GOS_CMD_CREATE_COMMAND(phy, mdio_read,            "phy_mdio_read",             "prd", 1, 1, true);
GOS_CMD_CREATE_COMMAND(phy, mdio_write,           "phy_mdio_write",            "pwr", 2, 2, false);


/*************************************************************************************************/
void gos_app_init(void)
{
  gos_result_t result = GOS_SUCCESS;

  // Initialize the default settings for this demo...this will enable the ethernet interface
  if(GOS_FAILED(result, gos_load_app_settings_once("settings.ini", DEMO_SETTINGS_MAGIC)))
  {
    GOS_LOG("Failed to load settings, err:%d", result);
  }

  GOS_LOG(APPLICATION_START_LINE);
  
  GOS_LOG("To read a register type:  phy_mdio_read <offset>");
  GOS_LOG("To write a register type: phy_mdio_write <offset> <hex_value)\r\n");
}


/*************************************************************************************************/
GOS_DEFINE_COMMAND(phy, mdio_read)
{ gos_cmd_result_t command_result = GOS_CMD_EXECUTE_AOK;
  gos_result_t result = GOS_SUCCESS;
  uint16_t reg_value;
 
	GOS_CMD_PARSE_INT_ARG(uint32_t, offset, argv[0], 0, 0x1F);    

  if(GOS_FAILED(result, ethernet_phy_read_reg(offset, &reg_value)))
  { if(result == GOS_TIMEOUT)
  	  command_result = GOS_CMD_TIMEOUT;
  	else
  		command_result = GOS_CMD_FAILED;
  }
  else
  { gos_cmd_print_hex_uint32_response((uint32_t)reg_value, 0);
  }

  return command_result;
}


/*************************************************************************************************/
GOS_DEFINE_COMMAND(phy, mdio_write)
{ gos_cmd_result_t command_result = GOS_CMD_EXECUTE_AOK;
  gos_result_t result = GOS_SUCCESS;
	
	GOS_CMD_PARSE_INT_ARG(uint32_t, offset, argv[0], 0, 0x1F);    
  GOS_CMD_PARSE_INT_ARG(uint32_t, value, argv[1], 0, 0xFFFF);    
    
	if(GOS_FAILED(result, ethernet_phy_write_reg(offset, value)))
  { if(result == GOS_TIMEOUT)
  	  command_result = GOS_CMD_TIMEOUT;
  	else
  		command_result = GOS_CMD_FAILED;
  }

  return command_result;

}


/*************************************************************************************************/
static gos_result_t ethernet_phy_write_reg(uint8_t reg, uint16_t value)
{
    gos_result_t result = GOS_TIMEOUT;
    const uint32_t start_time = gos_rtos_get_time();
    const uint32_t phy_command = PHYMNGMNT_OPERATION(1) | \
                                 PHYMNGMNT_CLAUSE_22    | \
                                 PHYMNGMNT_MUST10(2)    | \
                                 PHYMNGMNT_PHYADDR(PHY_ADDRESS) | \
                                 PHYMNGMNT_REGADDR(reg) | \
                                 PHYMNGMNT_DATA(value);

    ETH->PHYMNGMNT = phy_command;

    while((gos_rtos_get_time() - start_time) < 100)
    {
        if(ETH->NETWORKSTATUS & ETH_NETWORKSTATUS_MANDONE)
        {
            result = GOS_SUCCESS;
            break;
        }
        gos_rtos_delay_milliseconds(1);
    }

    return result;
}


/*************************************************************************************************/
static gos_result_t ethernet_phy_read_reg(uint8_t reg, uint16_t *value_ptr)
{
    gos_result_t result = GOS_TIMEOUT;
    const uint32_t start_time = gos_rtos_get_time();
    const uint32_t phy_command = PHYMNGMNT_OPERATION(2) | \
                                 PHYMNGMNT_CLAUSE_22    | \
                                 PHYMNGMNT_MUST10(2)    | \
                                 PHYMNGMNT_PHYADDR(PHY_ADDRESS) | \
                                 PHYMNGMNT_REGADDR(reg) | \
                                 PHYMNGMNT_DATA(0);

    ETH->PHYMNGMNT = phy_command;

    while((gos_rtos_get_time() - start_time) < 100)
    {
        if(ETH->NETWORKSTATUS & ETH_NETWORKSTATUS_MANDONE)
        {
            result = GOS_SUCCESS;
            *value_ptr = (ETH->PHYMNGMNT & _ETH_PHYMNGMNT_PHYRWDATA_MASK);
            break;
        }
        gos_rtos_delay_milliseconds(1);
    }

    return result;
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
