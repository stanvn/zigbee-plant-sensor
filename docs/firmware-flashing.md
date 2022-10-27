# Flashing the firmware
You can download the latest firmware from the [release](https://github.com/stanvn/zigbee-plant-sensor/releases) page. 

The nRF52840 microcontroller uses SWD for programming, so basically any programming supporting SWD can be used to flash the firmware. I recommend using a [J-Link edu mini](https://www.adafruit.com/product/3571) or a [nRF52840 DK](https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dk)

>**Note:** Some nRF modules ship with a ready-only flash. This is no problem for a J-Link (or nRF52840 DK) but it can lead to problems when you are using a ST-Link (V2)


## B-Parasite connection
To program the B-Parasite you can either solder wires to the programming pins or use a [6-pin pogo clamp](https://www.aliexpress.com/item/1005003667993025.html?pdp_npi=2%40dis%21EUR%21€%2011%2C28%21€%208%2C45%21%21%21%21%21%40211b423c16668977269877020ef735%2112000026719919447%21btf&_t=pvid:bf1f0be6-8bd6-4cfb-a538-c67a0824794a&afTraceInfo=1005003667993025__pc__c_ppc_item_bridge__xxxxxx__1666897727&spm=a2g0o.ppclist.product.mainProduct) as shown in the image below. 

![Connecting pogo clamp](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/images/pogopins.jpg)

> **IMPORTANT: always remove the battery before you connect the programmer**

Next connect the pins to the programmer as follows:

| B-Parasite | Programmer |
| ---------- | ---------- |
| Vcc        | +3.3V      |
| GND        | GND        |
| RST        | RST        |
| IO         | SWDIO      |
| CLK        | SWDCLK     |

## Programming using J-Link or nRF52840 DK
The nRF52840 can be used as a J-Link programmer if you connect it like this:
![nrf52 DK Connection](https://github.dev/stanvn/zigbee-plant-sensor/blob/main/docs/images/nrf52-dk-connection.png)

For other J-Links use the connections table above.

Once the programmer is connected and you have downloaded the .hex file from the [release](https://github.com/stanvn/zigbee-plant-sensor/releases) page, you can flash the file onto the microcontroller using [nRF Connect Programmer](https://infocenter.nordicsemi.com/index.jsp?topic=/struct_nrftools/struct/nrftools_nrfconnect.html). There documentation describes in details how to flash the hex file.
 