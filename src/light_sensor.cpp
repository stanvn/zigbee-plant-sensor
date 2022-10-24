#include <logging/log.h>
#include "light_sensor.hpp"

LOG_MODULE_REGISTER(light_sensor, CONFIG_LOG_LIGHT_SENSOR_LEVEL);

#define PRST_ADC_RESOLUTION 10

light_sensor_c::light_sensor_c(adc_c* adc, const struct gpio_dt_spec* photo_v_pin):
  m_adc(adc),m_photo_v_pin(photo_v_pin){}

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
  const float phototransistor_resistor = 470.0f;
  const float current_sun = 3.59e-3f;
   // Assuming 10000 lux for the saturation test. Calibration with a proper light
  // meter would be better.
  const float lux_sun = 10000.0f;
  const float current = voltage / phototransistor_resistor;
  const uint16_t brightness = (uint16_t)MAX(0, MIN(lux_sun * current / current_sun, UINT16_MAX));
  return brightness;
}
