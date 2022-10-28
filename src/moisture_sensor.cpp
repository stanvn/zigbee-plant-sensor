#include "moisture_sensor.hpp"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>
#include <math.h>
#include "logging/log.h"
#include "pm/device.h"
#include "pm/pm.h"



#define PWM_PERIOD_NS 3000

LOG_MODULE_REGISTER(moisture_sensor, CONFIG_LOG_MOISTURE_SENSOR_LEVEL);

moisture_sensor_c::moisture_sensor_c(adc_c* adc, const struct pwm_dt_spec* pwm, const struct gpio_dt_spec* discharge_pin)
  :m_adc(adc), m_pwm(pwm), m_discharge_pin(discharge_pin)
{
}

/**
* @brief initializes the soil moisture sensor
* @return STATUS_SUCCESS (0) on success
* @return positive error code otherwise
**/
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

/**
* @brief Get a readout from the soil moisture sensor
* @param battery_mv battery level in millivolts
* @return >= 0 with the readout value
* @return < 0 with a negative error message
**/
int8_t moisture_sensor_c::read(int32_t battery_mv){
  gpio_pin_set_dt(m_discharge_pin, 1);
  pwm_set_dt(m_pwm, PWM_PERIOD_NS, PWM_PERIOD_NS / 2U);
  k_msleep(2);
  int32_t adc_value = m_adc->read();
  pwm_set_dt(m_pwm, 0, 0);
  gpio_pin_set_dt(m_discharge_pin, 0);

  LOG_DBG("Sensor value: %d mV", adc_value); 

  return get_moisture_percentage(battery_mv, adc_value);
}

/**
* @brief convert the raw sensor reading to percentages
* @param bat_mv battery level in milli volts
* @param adc_reading the raw sensor valu
* @return soil moisture percentage
**/
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
