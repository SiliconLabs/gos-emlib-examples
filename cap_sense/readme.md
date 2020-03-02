This example illustrates how to use the low-level peripheral library EMLIB from a Gecko OS application to access the Capacitive Sense (CSEN) peripheral of the WGM160P.  

This example is designed to run on the [Hurricane Board](https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/hurricane/) which includes a single touch sensor pad.  The standard [WGM160P development kit](https://docs.silabs.com/gecko-os/4/standard/latest/hardware/silabs/wgm160p/) does not include a touch sensor.


> **NOTE:**  This example uses EMLIB from within Gecko OS.  EMLIB should only be used when Gecko OS does not provide the needed API to perform a specific function.  Care must be taken not to create resource conflicts with Gecko OS.  There is no guarantee that code relying directly on emlib will be compatible with future version of Gecko OS.  More information can be found at: [Using EMLIB in a Gecko OS Application](https://docs.silabs.com/gecko-os/4/standard/latest/sdk/development/using-emlib).
