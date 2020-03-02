This example illustrates how to use the low-level peripheral library EMLIB from a Gecko OS application to configure an internal Op Amp as a unity gain buffer and route the Op Amp output to an ADC.  The example uses PE15 as the input connected to the unity gain opamp.  The opamp output is then routed to PE14 which is connected to the ADC.  The example is designed for the WGM160P WSTK.  To use this example connect a voltage source to PE15 and leave PE14 disconnected.      

* PE14 = WSTK EXP12 = GOS_GPIO_0 = GOS_ADC_0
* PE15 = WSTK EXP14 = GOS_GPIO_1

Detailed pin mappings for the WGM160P can be found here: [https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections](https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/wgm160p-wstk-peripherals#wgm160p-peripheral-connections)

> **NOTE:**  This example uses EMLIB from within Gecko OS.  EMLIB should only be used when Gecko OS does not provide the needed API to perform a specific function.  Care must be taken not to create resource conflicts with Gecko OS.  There is no guarantee that code relying directly on emlib will be compatible with future version of Gecko OS.  More information can be found at: [Using EMLIB in a Gecko OS Application](https://docs.silabs.com/gecko-os/4/standard/latest/sdk/development/using-emlib).
