#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include "zigbee.hpp"
#include "battery.hpp"

extern "C" {
#include <addons/zcl/zb_zcl_temp_measurement_addons.h>
#include "zb_plant_sensor.h"
#include "zb_zcl_power_config.h"
#include "zboss_api_zdo.h"


#define LED0_NODE DT_ALIAS(led0)

  const struct gpio_dt_spec led_spec =  GPIO_DT_SPEC_GET(LED0_NODE, gpios);
  static bool joined = false;

  LOG_MODULE_REGISTER(zigbee, CONFIG_LOG_DEFAULT_LEVEL);

  /* Main application customizable context.
   * Stores all settings and static values.
   */
  struct zb_device_ctx {
    zb_zcl_basic_attrs_ext_t basic_attr;
    zb_zcl_identify_attrs_t identify_attr;
    zb_zcl_temp_measurement_attrs_t temp_measure_attrs;
    zb_zcl_rel_humidity_measurement_attr_t humidity_measurement_attrs;
    zb_zcl_rel_humidity_measurement_attr_t soil_moisture_measurement_attrs;
    zb_zcl_illuminance_measurement_attr_t illum_attrs;
    zb_zcl_power_config_attr_t power_config_attr;
  };

  /* Zigbee device application context storage. */
  static struct zb_device_ctx dev_ctx;

  // Create identify cluster attibute list
  ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(
      identify_attr_list,
      &dev_ctx.identify_attr.identify_time);


  // Create basic cluster attribute list
  ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(
      basic_attr_list,
      &dev_ctx.basic_attr.zcl_version,
      &dev_ctx.basic_attr.app_version,
      &dev_ctx.basic_attr.stack_version,
      &dev_ctx.basic_attr.hw_version,
      dev_ctx.basic_attr.mf_name,
      dev_ctx.basic_attr.model_id,
      dev_ctx.basic_attr.date_code,
      &dev_ctx.basic_attr.power_source,
      dev_ctx.basic_attr.location_id,
      &dev_ctx.basic_attr.ph_env,
      dev_ctx.basic_attr.sw_ver);

  // Create temperature measurement cluster attribute list
  ZB_ZCL_DECLARE_TEMP_MEASUREMENT_ATTRIB_LIST(
      temp_measurement_attr_list,
      &dev_ctx.temp_measure_attrs.measure_value,
      &dev_ctx.temp_measure_attrs.min_measure_value,
      &dev_ctx.temp_measure_attrs.max_measure_value,
      &dev_ctx.temp_measure_attrs.tolerance);

  // Create humidity measurement cluster attribute list
  ZB_ZCL_DECLARE_REL_HUMIDITY_MEASUREMENT_ATTRIB_LIST(
      humidity_measurement_attr_list,
      &dev_ctx.humidity_measurement_attrs.measure_value,
      &dev_ctx.humidity_measurement_attrs.min_measure_value,
      &dev_ctx.humidity_measurement_attrs.max_measure_value
      );

  // Create soil moisture measurement cluster attribure list
  ZB_ZCL_DECLARE_REL_HUMIDITY_MEASUREMENT_ATTRIB_LIST(
      soil_moisture_measurement_attr_list,
      &dev_ctx.soil_moisture_measurement_attrs.measure_value,
      &dev_ctx.soil_moisture_measurement_attrs.min_measure_value,
      &dev_ctx.soil_moisture_measurement_attrs.max_measure_value
      );

  // Create illuminance measurement cluster attribute list
  ZB_ZCL_DECLARE_ILLUMINANCE_MEASUREMENT_ATTRIB_LIST(
      illum_measure_attr_list,
      &dev_ctx.illum_attrs.measure_value,
      &dev_ctx.illum_attrs.min_measure_value,
      &dev_ctx.illum_attrs.max_measure_value);

  // Create power configuration cluster attribute list
  ZB_ZCL_DECLARE_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(
      power_config_attr_list,
      &dev_ctx.power_config_attr.battery_voltage,
      &dev_ctx.power_config_attr.battery_size,
      &dev_ctx.power_config_attr.battery_quantity,
      &dev_ctx.power_config_attr.battery_rated_voltage,
      &dev_ctx.power_config_attr.battery_alarm_mask,
      &dev_ctx.power_config_attr.battery_voltage_min_threshold,
      &dev_ctx.power_config_attr.battery_percentage_remaining,
      &dev_ctx.power_config_attr.battery_voltage_threshold1,
      &dev_ctx.power_config_attr.battery_voltage_threshold2,
      &dev_ctx.power_config_attr.battery_voltage_threshold3,
      &dev_ctx.power_config_attr.battery_percentage_min_threshold,
      &dev_ctx.power_config_attr.battery_percentage_threshold1,
      &dev_ctx.power_config_attr.battery_percentage_threshold2,
      &dev_ctx.power_config_attr.battery_percentage_threshold3,
      &dev_ctx.power_config_attr.battery_alarm_state
      );

  // Declare the plant sensor endpoint cluster list
  ZB_DECLARE_PLANT_SENSOR_CLUSTER_LIST(
      plant_sensor_clusters,
      basic_attr_list,
      identify_attr_list,
      temp_measurement_attr_list,
      humidity_measurement_attr_list,
      soil_moisture_measurement_attr_list,
      illum_measure_attr_list,
      power_config_attr_list
      );

  // Declare the plant sensor endpoint
  ZB_ZCL_DECLARE_PLANT_SENSOR_EP(
      plant_sensor_ep,
      PLANT_SENSOR_ENDPOINT,
      plant_sensor_clusters);

  // Attacht the app context to the plant sensor endpoint
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(app_sensor_ctx, plant_sensor_ep);

  void led_blink_timer_cb(struct k_timer *timer_id);

  K_TIMER_DEFINE(led_blink_timer, led_blink_timer_cb, NULL);


  /** @brief toggles the led when timer expires
   *
   *  @param timer ID
   */
  void led_blink_timer_cb(struct k_timer *timer_id){
    gpio_pin_toggle_dt(&led_spec);
  }

  /**@brief Function to handle identify notification events on the first endpoint.
   *
   * @param  bufid  Unused parameter, required by ZBOSS scheduler API.
   */
  static void identify_cb(zb_bufid_t bufid)
  {
    zb_ret_t zb_err_code;
    if (bufid) {
      LOG_INF("Identify on");
      /* Schedule a self-scheduling function that will toggle the LED. */
      gpio_pin_set_dt(&led_spec, 0);
      // Start blinking the led to indicate that the device is configuring
      k_timer_stop(&led_blink_timer);
      k_timer_start(&led_blink_timer, K_MSEC(1000), K_MSEC(1000));
    }
    else {
      LOG_INF("Identify off");
      /* Cancel the toggling function alarm and turn off LED. */
      k_timer_stop(&led_blink_timer);
      ZVUNUSED(zb_err_code);

      /* Update network status/idenitfication LED. */
      if (ZB_JOINED()) {
        gpio_pin_set_dt(&led_spec, 0);
      } else {
        gpio_pin_set_dt(&led_spec, 1);
      }
    }
  }


  /**
   * @brief Zigbee stack event handler.
   *
   * @param[in]   bufid   Reference to the Zigbee stack buffer
   *                      used to pass signal.
   */
  void zboss_signal_handler(zb_bufid_t bufid)
  {
    /* Update network status LED. */
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);

    static bool steering_success = false;

    switch (sig) {
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        // Fall through
      case ZB_BDB_SIGNAL_STEERING:
        if(status == RET_OK){
          steering_success = true;
          gpio_pin_set_dt(&led_spec, 0);
          // Start blinking the led to indicate that the device is configuring
          k_timer_stop(&led_blink_timer);
          k_timer_start(&led_blink_timer, K_MSEC(500), K_MSEC(500));
        }
        else{
          // Start the error blink
          joined = false;
          k_timer_stop(&led_blink_timer);
          k_timer_start(&led_blink_timer, K_MSEC(100), K_MSEC(100));
        }
        break;
      case ZB_ZDO_SIGNAL_LEAVE:
        /* Update network status LED */
        steering_success = false;
        joined = false;
        gpio_pin_set_dt(&led_spec, 1);
        break;
      case ZB_COMMON_SIGNAL_CAN_SLEEP:
        if(steering_success && !joined){ // Joining success
          // Reduce the data polling to save power
          zb_zdo_pim_set_long_poll_interval(CONFIG_ZIGBEE_DATA_POLL_INTERVAL);
          k_timer_stop(&led_blink_timer);
          gpio_pin_set_dt(&led_spec, 0);
          joined = true;
        }
        zb_sleep_now();
        break;

      default:
        break;
    }

    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));

    /* No application-specific behavior is required.
     * Call default signal handler.
     */

    /* All callbacks should either reuse or free passed buffers.
     * If bufid == 0, the buffer is invalid (not passed).
     */
    if (bufid) {
      zb_buf_free(bufid);
    }
  }

  /**
   * @brief initialize all the cluster attributes with a default value.
   **/
  static void app_clusters_attr_init(void)
  {
    /* Basic cluster attributes data */
    dev_ctx.basic_attr.zcl_version = ZB_ZCL_VERSION;
    dev_ctx.basic_attr.power_source = APP_INIT_BASIC_POWER_SOURCE;
    dev_ctx.basic_attr.ph_env = APP_INIT_BASIC_PH_ENV;
    dev_ctx.basic_attr.app_version = APP_INIT_BASIC_APP_VERSION;
    dev_ctx.basic_attr.stack_version = APP_INIT_BASIC_STACK_VERSION;
    dev_ctx.basic_attr.hw_version = APP_INIT_BASIC_HW_VERSION;

    /* Identify cluster attributes data. */
    dev_ctx.identify_attr.identify_time =
      ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.mf_name,
        APP_INIT_BASIC_MANUF_NAME,
        ZB_ZCL_STRING_CONST_SIZE(APP_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.model_id,
        APP_INIT_BASIC_MODEL_ID,
        ZB_ZCL_STRING_CONST_SIZE(APP_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.date_code,
        APP_INIT_BASIC_DATE_CODE,
        ZB_ZCL_STRING_CONST_SIZE(APP_INIT_BASIC_DATE_CODE));

    ZB_ZCL_SET_STRING_VAL(
        dev_ctx.basic_attr.location_id,
        APP_INIT_BASIC_LOCATION_DESC,
        ZB_ZCL_STRING_CONST_SIZE(APP_INIT_BASIC_LOCATION_DESC));

    dev_ctx.power_config_attr.battery_voltage = ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_INVALID;
    dev_ctx.power_config_attr.battery_percentage_min_threshold = (uint8_t)(BATTERY_ALARM_MV / ZCL_BATTERY_VOLTAGE_VALUE_DIVIDER) ;

  }
} // extern "C"

/**
 * @brief Initialize and start the zigbee thread
 **/
status_code_t zigbee_start(){

  zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
  zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(300000));
  zigbee_configure_sleepy_behavior(true);
  zb_set_rx_on_when_idle(ZB_FALSE);

  /* Register device context (endpoints). */
  ZB_AF_REGISTER_DEVICE_CTX(&app_sensor_ctx);

  app_clusters_attr_init();

  if (!device_is_ready(led_spec.port)) {
    LOG_ERR("LED is not ready");
    return STATUS_ERROR_DEVICE_NOT_READY;
  }

  int32_t ret = gpio_pin_configure_dt(&led_spec, GPIO_OUTPUT);
  if (ret < 0) {
    LOG_ERR("Failed to configure LED");
    return STATUS_ERROR_IO;
  }
  gpio_pin_set_dt(&led_spec, 1);

  /* Register handlers to identify notifications */
  ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(PLANT_SENSOR_ENDPOINT, identify_cb);
  zigbee_erase_persistent_storage(ZB_FALSE);


  /* Start Zigbee default thread */
  zigbee_enable();

  return STATUS_SUCCESS;
}
