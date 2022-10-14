#pragma once

#include <zephyr.h>
#include "adc.hpp"
#include <zephyr/drivers/pwm.h>
#include <drivers/gpio.h>

/* The moisture level is calculated based on the ADC sensor
 * reading and the battery level. Lower battery level also means 
 * lower ADC reading. There are two equations:
 * min = EQ_MIN_A*battery_level + EQ_MIN_B
 * max = EQ_MAX_A*battery_level + EQ_MAX_B
 * Where min is the minimal expected sensor value when fully dry
 * and max is the maximal expected sensor value when fully wet
 */
#define EQ_MIN_A 0.6341
#define EQ_MIN_B -353.22

#define EQ_MAX_A 0.4688
#define EQ_MAX_B -235.07


class moisture_sensor_c{
  public:
    moisture_sensor_c(adc_c* adc, const struct pwm_dt_spec* pwm, const struct gpio_dt_spec* discharge_pin);
    status_code_t init();
    int32_t read();

  private:
    adc_c* m_adc;
    const struct pwm_dt_spec* m_pwm;
    const struct gpio_dt_spec* m_discharge_pin;
};
