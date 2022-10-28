#pragma once

#include <zephyr.h>
#include <drivers/gpio.h>
#include "adc.hpp"
#include "status_codes.hpp"

/**
* @brief Initialize and readout the phototransistor
**/
class light_sensor_c{
  public:
    light_sensor_c(adc_c* adc, const struct gpio_dt_spec* photo_v_pin);
    status_code_t init();
    int32_t read();
  private:
    adc_c *m_adc;
    const struct gpio_dt_spec* m_photo_v_pin;
};
