This example illustrates how to use the low-level peripheral library EMLIB from a Gecko OS application to configure and operate a timer to create a single pulse on GOS_GPIO_2 (PA0) that is triggered by GOS_GPIO_3 (PA1).  The output pulse is triggered by both edges of the source.  This example can control both the delay (time between source edge and the start of the pulse) and the pulse width.

This is achieved using a hardware timer and two compare capture channels.  It is fully hardware driven.

There are two modes of operation for this example: 1) PWM used to create the source 2) button push used to create the source.  The mode is chosen using the USE_PWM_SOURCE compile switch.  In both cases GOS_GPIO_6 is used to generate the source signal.  To operate the example you should use a wire to connect GOS_GPIO_6 to GOS_GPIO_3 which are available on the WSTK EXP header at pins EXP7 and EXP6
respectively.

In the button mode (USE_PWM_SOURCE == 0) a button push will toggle the source so you should see one pulse per button push.

In PWM mode (USE_PWM_SOURCE == 1), the PWM output is started immediately so the output pulse should be continuous.  In this mode a button push will perform a NVM save.  This demonstrates that the pulse output is not interrupted by the NVM save process.  GOS_GPIO_1 will be driver during the NVM save to allow the user to trigger a scope.

The GPIOs used in this example are summarized below:
* GOS_GPIO_2 = PA0  = WSTK EXP4   - Pulse output
* GOS_GPIO_3 = PA1  = WSTK EXP6   - Pulse trigger input
* GOS_GPIO_6 = PA4  = WSTK EXP7   - Source of the trigger.  Connect this line to the pulse trigger input
* GOS_GPIO_1 = PE15 = WSTK EXP14  - Used to trigger a scope during NVM save in PWM mode (USE_PWM_SOURCE == 1)

Further details of the WGM160P pin mappings can be found here: [https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections](https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections)

> **NOTE:**  This example uses EMLIB from within Gecko OS.  EMLIB should only be used when Gecko OS does not provide the needed API to perform a specific function.  Care must be taken not to create resource conflicts with Gecko OS.  There is no guarantee that code relying directly on emlib will be compatible with future version of Gecko OS.  More information can be found at: [Using EMLIB in a Gecko OS Application](https://docs.silabs.com/gecko-os/4/standard/latest/sdk/development/using-emlib).
