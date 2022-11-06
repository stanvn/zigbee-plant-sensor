#include <logging/log.h>
#include "light_sensor.hpp"

LOG_MODULE_REGISTER(light_sensor, CONFIG_LOG_LIGHT_SENSOR_LEVEL);

light_sensor_c::light_sensor_c(adc_c* adc, const struct gpio_dt_spec* photo_v_pin):
  m_adc(adc),m_photo_v_pin(photo_v_pin){}

/**
* @brief initializes the light sensor
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
status_code_t light_sensor_c::init(){
  if(!device_is_ready(m_photo_v_pin->port)){
    return STATUS_ERROR_DEVICE_NOT_READY;
  }
  status_code_t st = gpio_pin_configure_dt(m_photo_v_pin, GPIO_OUTPUT);
  if(st != STATUS_SUCCESS){
    return STATUS_ERROR_IO;
  }
  return m_adc->init();
}

/**
* @brief Get a readout from the light sensor
* @return >= 0 with the readout value
* @return < 0 with a negative error message
**/
int32_t light_sensor_c::read(){
  gpio_pin_set_dt(m_photo_v_pin, 1);
  k_msleep(1);
  int32_t raw_mV = m_adc->read();
  LOG_DBG("Raw: %d mV", raw_mV);
  gpio_pin_set_dt(m_photo_v_pin, 0);
  if(raw_mV < 0){
    // minus values are errors
    return raw_mV;
  }
  const double voltage = (double)raw_mV / 1000.0;
  const double phototransistor_resistor = 470.0f;
  // Reference values are based on the datasheet where the current at
  // 10000 lux should be 1500 uA
  const double lux_ref = 10000.0f;
  const double current_ref = 3.59e-3f;

  const double current = voltage / phototransistor_resistor;
  const uint16_t brightness = (uint16_t)MAX(0, MIN(lux_ref * current / current_ref, UINT16_MAX));
  return brightness;
}
