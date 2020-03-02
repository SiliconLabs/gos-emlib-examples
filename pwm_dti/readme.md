This example illustrates how to use the low-level peripheral library EMLIB from a Gecko OS application to create a 3 channel (6 signals) Pulse Width Modulation (PWM) with Delay Time Insertion (DTI) controller.  
The GPIOs and timers used for each channel are summarized below:

| CHANNEL | GOS_GPIO_# | TIMER# | CC CHAN | LOC# | PIN | EXP   |
| --------|------------|--------|---------|------|-----|-------|
|   1     |    2       | TIMER0 | CC0     |  0   | PA0 | EXP4  |
|   --    |    5       | TIMER0 | CDTI0   |  0   | PA3 | EXP10 |
|   2     |    3       | TIMER0 | CC1     |  0   | PA1 | EXP6  |
|   --    |    6       | TIMER0 | CDTI1   |  0   | PA4 | EXP7  |
|   3     |    4       | TIMER0 | CC2     |  0   | PA2 | EXP8  |
|   --    |    7       | TIMER0 | CDTI2   |  0   | PA5 | EXP9  |

Detailed pin mappings for the WGM160P can be found here: [https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections](https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections)

> **NOTE:**  This example uses EMLIB from within Gecko OS.  EMLIB should only be used when Gecko OS does not provide the needed API to perform a specific function.  Care must be taken not to create resource conflicts with Gecko OS.  There is no guarantee that code relying directly on emlib will be compatible with future version of Gecko OS.  More information can be found at: [Using EMLIB in a Gecko OS Application](https://docs.silabs.com/gecko-os/4/standard/latest/sdk/development/using-emlib).
