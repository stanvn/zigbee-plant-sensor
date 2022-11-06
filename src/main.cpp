// Code for Zigbee plant sensors based on the nRF52 microcontroller    
// Copyright (C) 2022 Stan van Nieuwamerongen 
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include <zephyr.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/pwm.h>
#include <drivers/gpio.h>
#include <ram_pwrdn.h>

#include <sys/printk.h>
#include "logging/log.h"
#include "battery.hpp"
#include "status_codes.hpp"
#include "adc.hpp"
#include "moisture_sensor.hpp"
#include "light_sensor.hpp"
#include "zigbee.hpp"
#include "math.h"

extern "C" {
#include <addons/zcl/zb_zcl_temp_measurement_addons.h>
#include "zb_plant_sensor.h"
#include "zb_zcl_soil_moisture.h"
}


LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

#define SHTC_NODE DT_ALIAS(temp_sensor)
#define CONFIG_SHTC3_DEV_NAME DT_LABEL(SHTC3)


static const struct pwm_dt_spec moisture_pwm = PWM_DT_SPEC_GET(DT_ALIAS(sensor_pwm));
static const struct gpio_dt_spec discharge_pin = GPIO_DT_SPEC_GET(DT_ALIAS(fast_discharge_en), gpios);
static const struct gpio_dt_spec photo_v_pin = GPIO_DT_SPEC_GET(DT_ALIAS(photo_v), gpios);

adc_c moisture_adc(2);
adc_c light_sensor_adc(1);
moisture_sensor_c moisture_sensor(&moisture_adc, &moisture_pwm, &discharge_pin);
light_sensor_c light_sensor(&light_sensor_adc, &photo_v_pin);

const struct device* shtc3;

/**
* @brief initializes the temeperature and humidity sensor
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
static status_code_t init_shtc3_device(void){
  shtc3 = DEVICE_DT_GET(SHTC_NODE);
  if(shtc3 == NULL){
    /* No such node, or the node does not have status "okay". */
    printk("\nError: no device found.\n");
    return STATUS_ERROR_DEVICE_NOT_FOUND;
  }
  if(!device_is_ready(shtc3)){
    printk("\nError: Device \"%s\" is not ready; "
        "check the driver initialization logs for errors.\n",
        shtc3->name);
    return STATUS_ERROR_DEVICE_NOT_READY;
  }
  LOG_INF("Found device \"%s\", getting sensor data\n", shtc3->name);
  return STATUS_SUCCESS;
}

/**
* @brief Readout the temperature and update the zigbee cluster attribute 
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t update_temperature(){
  struct sensor_value temp;
  int16_t temperature_attribute = 0;
  double measured_temperature;

  status_code_t st = sensor_channel_get(shtc3, SENSOR_CHAN_AMBIENT_TEMP, &temp);
  measured_temperature = sensor_value_to_double(&temp);
  if(st == STATUS_SUCCESS){
    temperature_attribute = (int16_t)
      (measured_temperature *
       ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
    LOG_INF("Temp: %d", temperature_attribute);

    zb_zcl_status_t status = zb_zcl_set_attr_val(
        PLANT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&temperature_attribute,
        ZB_FALSE);

    if (status) {
      LOG_ERR("Failed to set ZCL attribute: %d", status);
      return -status;
    }

  }
  return -st;
}

/**
* @brief Readout the humidity and update the zigbee cluster attribute 
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t update_humidity(){
  struct sensor_value hum;
  int16_t humidity_attribute = 0;
  double measured_humidity;

  status_code_t st = sensor_channel_get(shtc3, SENSOR_CHAN_HUMIDITY, &hum);
  measured_humidity = sensor_value_to_double(&hum);
  if(st == STATUS_SUCCESS){
    humidity_attribute = (int16_t)
      (measured_humidity *
       ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
    LOG_INF("Hum: %d.%06d\n", hum.val1, hum.val2);

    zb_zcl_status_t status = zb_zcl_set_attr_val(
        PLANT_SENSOR_ENDPOINT,
        ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,
        ZB_ZCL_CLUSTER_SERVER_ROLE,
        ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
        (zb_uint8_t *)&humidity_attribute,
        ZB_FALSE);

    if (status) {
      LOG_ERR("Failed to set ZCL attribute: %d", status);
      return -status;
    }

  }
  return -st;
}

/**
* @brief Readout the soil moisture and update the zigbee cluster attribute 
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t update_soil_moisture(int32_t battery_mv){
  int8_t moisture_val = moisture_sensor.read(battery_mv);
  if(moisture_val < 0){
    LOG_ERR("Failed to read soil moisture");
    return -moisture_val;
  }
  int16_t moisture_attribute = (int16_t)(moisture_val * ZCL_SOIL_MOISTURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER);
  LOG_INF("Soil moisture: %d\n", moisture_val);

  zb_zcl_status_t status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_SOIL_MOISTURE_MEASUREMENT,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID,
      (zb_uint8_t *)&moisture_attribute,
      ZB_FALSE);
  if (status) {
    LOG_ERR("Failed to set ZCL attribute: %d", status);
    return -status;
  }
  return STATUS_SUCCESS;
}

/**
* @brief Readout the light sensor and update the zigbee cluster attribute 
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t update_light_sensor(){
  int32_t value = light_sensor.read();
  if(value < 0){
    LOG_ERR("Failed to read from light sensor");
    return -value;
  }
  LOG_INF("Light: %d", value);
  int16_t light_sensor_attribute = (int16_t)(10000.0 * log10(value));
  LOG_DBG("Light attribute: %d", light_sensor_attribute);

  zb_zcl_status_t status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID,
      (zb_uint8_t *)&light_sensor_attribute,
      ZB_FALSE);


  if (status) {
    LOG_ERR("Failed to set ZCL attribute: %d", status);
    return -status;
  }
  return STATUS_SUCCESS;
}

/**
* @brief Readout the battery level and update the zigbee cluster attribute 
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t update_battery_state(int32_t battery_mv){
  if(battery_mv < 0){
    LOG_ERR("Failed to sample battery");
    return -battery_mv;
  }
  uint8_t battery_attribute = (uint8_t)(battery_mv / ZCL_BATTERY_VOLTAGE_VALUE_DIVIDER);
  zb_zcl_status_t status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID,
      (zb_uint8_t *)&battery_attribute,
      ZB_FALSE);

  if (status) {
    LOG_ERR("Failed to set ZCL attribute: %d", status);
    return -status;
  }

  uint8_t alarm_mask = 0;
  if(battery_mv < BATTERY_ALARM_MV)
    alarm_mask |= 1;
  if(battery_mv < BATTERY_THRESHOLD1_MV)
    alarm_mask |= 1 << 1;
  if(battery_mv < BATTERY_THRESHOLD2_MV)
    alarm_mask |= 1 << 2;
  if(battery_mv < BATTERY_THRESHOLD3_MV)
    alarm_mask |= 1 << 3;
  LOG_DBG("Battery alarm mask: %b", alarm_mask);

  status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID,
      (zb_uint8_t *)&alarm_mask,
      ZB_FALSE);

  if (status) {
    LOG_ERR("Failed to set ZCL attribute: %d", status);
    return -status;
  }

  status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID,
      (zb_uint8_t *)ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_OTHER,
      ZB_FALSE);

  status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID,
      (zb_uint8_t *)1,
      ZB_FALSE);

  uint8_t soc = (uint8_t)round(battery_state_of_charge(battery_mv) * 2);

  status = zb_zcl_set_attr_val(
      PLANT_SENSOR_ENDPOINT,
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
      ZB_ZCL_CLUSTER_SERVER_ROLE,
      ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID,
      (zb_uint8_t *)&soc,
      ZB_FALSE);
  
  if (status) {
    LOG_ERR("Failed to set ZCL attribute: %d", status);
    return -status;
  }

  return STATUS_SUCCESS;

}

int main(void)
{
  LOG_INF("Plant sensor application started!");
  check_error(battery_measure_enable(true), "Failed to start battery measurement");
  check_error(moisture_sensor.init(), "Failed to init moisture sensor");
  check_error(init_shtc3_device(), "Failed to init temperature sensor");
  check_error(light_sensor.init(), "Failed to init light sensor");

  /* Power off unused sections of RAM to lower device power consumption. */
  if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
    power_down_unused_ram();
  }

  zigbee_start();

  int32_t battery_mv = 0;

  while(true){
    battery_measure_enable(true);
    battery_mv = battery_sample();
    battery_measure_enable(false);
    sensor_sample_fetch(shtc3);
    update_temperature();
    update_humidity();
    update_soil_moisture(battery_mv);
    update_light_sensor();
    update_battery_state(battery_mv);

    // Zephyr is smart enough to put the device in deep sleep when
    // there is nothing running. So as long as we make sure not other
    // background tasks are running, the device will go to deep sleep
    k_msleep(CONFIG_ZIGBEE_UPDATE_PERIOD * 1000);
  }

  return 0;
}
