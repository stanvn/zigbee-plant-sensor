# Configuring Zigbee2MQTT
Zigbee2MQTT needs some configuration before it recognises the plant sensor. Luckily this is quit easy to do. 
1. Copy the [this](https://github.com/stanvn/zigbee-plant-sensor/blob/main/zigbee2mqtt/plant_sensor.js) javascript file in the same directory as you Zigbee2MQTT `configuration.yaml` file.
2. Add the following lines to your Zigbee2MQTT `configuration.yaml` file
    ```yaml
        external_converters:
          - plant_sensor.js
    ```
3. Restart Zigbee2MQTT

Zigbee2MQTT should now recognize the plant sensor.
