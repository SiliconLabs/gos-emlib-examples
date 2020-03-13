This example illustrates how to use the low-level peripheral library EMLIB from a Gecko OS application to directly read and write registers available on the MDIO interface of an Ethernet PHY device.

This example defines two custom commands that can be used from the device console to read and write PHY registers.  

**phy_mdio_read**

Performs an MDIO read

* Abbreviation: `prd`
* Syntax: `> phy_mdio_read <offset>`
* Example: `> phy_mdio_read 0`

**phy_mdio_write**

Performs an MDIO write

* Abbreviation: `pwr`
* Syntax: `> phy_mdio_write <offset> <hex_value>`
* Example: `> phy_mdio_write 0 0x3000`

This example automatically enables the ethernet interface.  See the project file `resources/settings.ini`.  The PHY device address is hardcoded in main.c.


> **NOTE:**  This example uses EMLIB from within Gecko OS.  EMLIB should only be used when Gecko OS does not provide the needed API to perform a specific function.  Care must be taken not to create resource conflicts with Gecko OS.  There is no guarantee that code relying directly on emlib will be compatible with future version of Gecko OS.  More information can be found at: [Using EMLIB in a Gecko OS Application](https://docs.silabs.com/gecko-os/4/standard/latest/sdk/development/using-emlib).
