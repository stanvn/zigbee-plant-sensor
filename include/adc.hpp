#pragma once
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include "status_codes.hpp"

/**
* @brief Read analog value with the ADC
**/
class adc_c{
  public:
    adc_c(uint16_t channel);
    status_code_t init();
    int32_t read();

  private:
    static const struct adc_dt_spec m_adc_channels[];
    struct adc_dt_spec m_adc_spec;
    int16_t m_buf;
    struct adc_sequence m_sequence;
};
