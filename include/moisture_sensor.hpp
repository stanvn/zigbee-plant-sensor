#pragma once

#include <zephyr.h>
#include "adc.hpp"
#include <zephyr/drivers/pwm.h>
#include <drivers/gpio.h>

/* The moisture level is calculated based on the ADC sensor
 * reading and the battery level. Lower battery level also means 
 * lower ADC reading. There are two equations:
 * dry = EQ_DRY_A*battery_level + EQ_DRY_B
 * wet = EQ_WET_A*battery_level + EQ_WET_B
 * Where dry is the maximal expected sensor value when fully dry
 * and max is the minimal expected sensor value when fully wet
 */
#define EQ_DRY_A 0.9321
#define EQ_DRY_B -449.87

#define EQ_WET_A 0.1298
#define EQ_WET_B -203.09

/**
* @brief Initialize and readout the soil moisture sensor
**/
class moisture_sensor_c{
  public:
    moisture_sensor_c(adc_c* adc, const struct pwm_dt_spec* pwm, const struct gpio_dt_spec* discharge_pin);
    status_code_t init();
    int8_t read(int32_t battery_mv);

  private:
    int8_t get_moisture_percentage(int32_t bat_mV, int32_t adc_reading);
    adc_c* m_adc;
    const struct pwm_dt_spec* m_pwm;
    const struct gpio_dt_spec* m_discharge_pin;
};
