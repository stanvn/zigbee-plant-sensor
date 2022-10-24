#include "moisture_sensor.hpp"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>
#include <math.h>
#include "battery.hpp"
#include "logging/log.h"
#include "pm/device.h"
#include "pm/pm.h"



#define PWM_PERIOD 512

LOG_MODULE_REGISTER(moisture_sensor, CONFIG_LOG_MOISTURE_SENSOR_LEVEL);

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

int8_t moisture_sensor_c::read(){
  gpio_pin_set_dt(m_discharge_pin, 0);
  pwm_set_dt(m_pwm, PWM_PERIOD, PWM_PERIOD / 2U);
  battery_measure_enable(true);
  k_msleep(5);
  int32_t adc_value = m_adc->read();
  pwm_set_dt(m_pwm, 0, 0);
  gpio_pin_set_dt(m_discharge_pin, 1);
  k_msleep(5);
  gpio_pin_set_dt(m_discharge_pin, 0);
  int32_t bat_mV = battery_sample();
  battery_measure_enable(false);

  LOG_DBG("Battery value: %d mV", bat_mV);
  LOG_DBG("Sensor value: %d mV", adc_value); 

  return get_moisture_percentage(bat_mV, adc_value);
}

int8_t moisture_sensor_c::get_moisture_percentage(int32_t bat_mV, int32_t adc_reading){
  float wet_value = EQ_WET_A * (float)bat_mV + EQ_WET_B;
  float dry_value = EQ_DRY_A * (float)bat_mV + EQ_DRY_B;
  if(adc_reading > dry_value){
    adc_reading = dry_value;
  }
  else if(adc_reading < wet_value){
    adc_reading = wet_value;
  }
  LOG_DBG("Dry: %d", (int32_t)dry_value);
  LOG_DBG("Wet: %d", (int32_t)wet_value);
  float percentage = ((adc_reading - wet_value) / (dry_value - wet_value));
  return (int8_t)round(100 - percentage*100); 
}
