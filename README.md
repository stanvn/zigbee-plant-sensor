![banner](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/images/zps-banner.png)

# Zigbee Plant Sensor for Zigbee2MQTT
An open source Zigbee plant sensor project developed with the nRF Connect SDK (Zephyr RTOS). It is designed for the [b-parasite](https://github.com/rbaron/b-parasite) hardware, however due to the flexibility of Zephyr it should be easy to port it to other hardware containing a capacitive soil moisture sensor.

## Features
- Works with Zigbee2MQTT
- Soil moisture measurements
- Illuminance measurements
- Temperature and humidity measurements  
- Battery voltage measurements and alarms 
- Developed with the nRF Connect SDK (Zephyr RTOS)

## Hardware
**TODO insert image**

### B-Parasite
The software is designed for the open source hardware [b-parasite](https://github.com/rbaron/b-parasite). [Rbaron](https://github.com/rbaron) did a great job designing a soil moisture sensor, however his software is only made for BLE. Fortunately, the hardware contains a nRF52840 chip which is also compatible with Zigbee. The only problem is that it does not have a button which might be useful for factory reset. There are however less ideal workarounds that do not require a button. 

### Complete hardware kit
I am developing my own hardware that is smal and suited for Zigbee (With a reset button). I am willing to sell completely assembled hardware if there is enough interest. If you're interested please fill in the form below. You will receive a email once they become available. Thank you.

**TODO: Add form link**

## Guides
- [Flashing the firmware](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/firmware-flashing.md)
- [Adding Zigbee2MQTT support](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/zigbee2mqtt.md#L12)
- Calibration (TODO)

## Coating
Straight from the factory PCBs are not coated on de sides. This might affect the reading in the future as water seeps inside. Therefore it is a good idea to make it water resistant with some coating. I have used [Kontakt 70](https://www.amazon.de/-/nl/dp/B000YIW7GS/ref=sr_1_4?__mk_nl_NL=ÅMÅŽÕÑ&crid=R4U07V394JKJ&keywords=kontakt+70&qid=1666814358&qu=eyJxc2MiOiIxLjk4IiwicXNhIjoiMS44MyIsInFzcCI6IjEuODgifQ%3D%3D&sprefix=kontakt+70%2Caps%2C66&sr=8-4), but I am still researching other alternatives. Let me know if you have better coating ideas.

The only problem with coating is that you probably need to recalibrate the sensor.

## Battery life
By default, data is reported every two minutes. In the worst case scenario, all attributes are updated and transmitted. The picture below shows the power consumption of two minuuts in this worse case scenario.

![power measurements](https://github.com/stanvn/zigbee-plant-sensor/blob/main/docs/images/power_measurements.png)

The measurements shows that the average power consumption is $14.53\text{uA}$. If we consider a CR2032 battery with a capacity of $210\text{mAh}$ we get $210/0.01453 = 14453$ hours or $602$ days! Even if the measurements are a bit off, we still have more than a year of battery life. 

## 3D printed case
Check the link below for an easy to 3D print case. It is designed for threaded inserts with a outer radius of 4 mm and 3 mm bolts.

**TODO add printables url**

## Known issues
- Battery voltage and alarms are not reported correctly. You can read them using the dev console in Zigbee2MQTT, but somehow the automatic reporting does not work with the Zigbee power config cluster.
- The hardware does not have a button therefore it cannot be factory reset. The overcome this, the joining process is done when the battery is insert. If the sensor has not joined a network yet or it cannot find a known network, it scans for other networks. In this way it is still possible to join a new network even if it is not factory reset.
- When firstly installed in a plant pot the moisture readings will go slightly up over the hours. Not sure if that is because it needs to settle in the soil first or if there is some drift in the ADC reading.
