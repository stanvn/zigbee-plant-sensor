![banner](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/images/zps-banner.png)

# Zigbee Plant Sensor
An open source Zigbee plant sensor for the nRF52 developed with the nRF Connect SDK (Zephyr RTOS). It is designed for the [b-parasite](https://github.com/rbaron/b-parasite) hardware, however due to the flexbility of Zephyr it should be easy to port it to other hardware containing a capasitive soil moisture sensor. 

## Features
- Works with Zigbee2MQTT
- Soil moisture sensor
- Illuminance sensor
- Temperature and humidity sensor
- Battery voltage measurements and alarms 
- Developed with the nRF Connect SDK (Zephyr RTOS)

## Hardware
**TODO insert image**

### B-Parasite
The software is designed for the open source hardware [b-parasite](https://github.com/rbaron/b-parasite). [Rbaron](https://github.com/rbaron) did a great job designing a soil moisture sensor. His software is made for BLE, however it contains a nRF52840 chip which is also compatible with Zigbee. The only problem is that it does not have a button which might be usefull for factory reset. This is not critical because there are workarounts. 

### Order hardware
I am making my own hardware is smaller and better suited for Zigbee (With a reset button) that the b-parasite hardware. I am willing to sell, if there is enough interest. If you're interested please fill in the form below. You will receive a email once they become available. Thank you.

## Guids
- [Flashing the firmware]()
- [Adding Zigbee2MQTT support]()
- [Calibration]()

## Coating

## Battery life
**TODO add screenshot**


## 3D printed case
Check the link below for an easy to 3D print case. It is designed for threaded inserts of 4 mm outer radius and 3 mm bolts.

**TODO add printables url**

## Known issues
- Battery voltage and alarams are not reported correctly. You can read them using the dev console in Zigbee2MQTT, but somehow the automatic reporting does not work with the power config cluster
- The hardware does not have a button therefor it cannot be fatory reset. After inserting the battery tries to connect to a already joined network, if it cannot be found, it will look for a new network to join.
- When firstly installed in a plant pot the moisture readings will go slightly up over the hours. Not sure if that is because it needs to seatle first or if there is some drift in the ADC reading.
