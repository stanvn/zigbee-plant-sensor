/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/pwm.h>
#include <drivers/gpio.h>

#include <sys/printk.h>
#include "logging/log.h"
#include "buildin-led.hpp"
#include "battery.hpp"
#include "status_codes.hpp"
#include "adc.hpp"
#include "moisture_sensor.hpp"


#define MIN_PERIOD PWM_SEC(1U) / 128U
#define BUF_SIZE 100

LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);


static const struct pwm_dt_spec moisture_pwm = PWM_DT_SPEC_GET(DT_ALIAS(sensor_pwm));
static const struct gpio_dt_spec discharge_pin = GPIO_DT_SPEC_GET(DT_ALIAS(fast_discharge_en), gpios);

adc_c moisture_adc(1);
moisture_sensor_c moisture_sensor(&moisture_adc, &moisture_pwm, &discharge_pin);

int main(void)
{
  LOG_INF("Application started!");
  buildin_led_init();
  check_error(battery_measure_enable(true), "Failed to start battery measurement");
  check_error(moisture_sensor.init(), "Failed to init moisture sensor");

  int32_t batt_buff[BUF_SIZE] = {0};
  int32_t sensor_buff[BUF_SIZE] = {0};
  uint16_t avg_counter = 0;

  while (true)
  {
    int batt_mV = battery_sample();
    int32_t moisture_level = moisture_sensor.read();
    if(moisture_level < 0){
      LOG_ERR("Failed to read moisture level: %d", -moisture_level);
      continue;
    }
    if (batt_mV < 0) {
      LOG_ERR("Failed to read battery voltage: %d",
          batt_mV);
      continue;
    }
    batt_buff[avg_counter] = batt_mV;
    sensor_buff[avg_counter] = moisture_level;
    avg_counter+=1;

    if(avg_counter >= BUF_SIZE){
      avg_counter = 0;
      uint32_t batt_sum = 0;
      uint32_t sensor_sum = 0;
      for(uint32_t i = 0; i < BUF_SIZE; i++){
        batt_sum+= batt_buff[i];
        sensor_sum+= sensor_buff[i];
      }
      LOG_INF("Battery: %d; Moisture: %d mV", batt_sum/BUF_SIZE, sensor_sum/BUF_SIZE);
      buildin_led_toggle();
    }


    k_msleep(100);


  }
  return 1;
}
