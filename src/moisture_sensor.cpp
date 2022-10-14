#include "moisture_sensor.hpp"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>

#define PWM_PERIOD 512

moisture_sensor_c::moisture_sensor_c(adc_c* adc, const struct pwm_dt_spec* pwm, const struct gpio_dt_spec* discharge_pin)
  :m_adc(adc), m_pwm(pwm), m_discharge_pin(discharge_pin)
{
}

status_code_t moisture_sensor_c::init(){
  status_code_t st = m_adc->init();
  if(st != STATUS_SUCCESS){
    return st;
  }
  if(!device_is_ready(m_pwm->dev)){
    return STATUS_ERROR_DEVICE_NOT_READY;
  }
  if(!device_is_ready(m_discharge_pin->port)){
    return STATUS_ERROR_DEVICE_NOT_READY;
  }
  st = gpio_pin_configure_dt(m_discharge_pin, GPIO_OUTPUT);
  if(st != STATUS_SUCCESS){
    return STATUS_ERROR_IO;
  }
  return STATUS_SUCCESS;
}

int32_t moisture_sensor_c::read(){
  gpio_pin_set_dt(m_discharge_pin, 0);
  pwm_set_dt(m_pwm, PWM_PERIOD, PWM_PERIOD / 2U);
  k_msleep(10);
  int32_t adc_value = m_adc->read();
  pwm_set_dt(m_pwm, PWM_PERIOD, 0);
  gpio_pin_set_dt(m_discharge_pin, 1);
  return adc_value;

}
