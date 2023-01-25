#include <device.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include "zigbee.hpp"
#include "battery.hpp"

#define LED0_NODE DT_ALIAS(led0)
#define FACTORY_RESET_TIMEOUT 5000
#define MAX_STEERING_COUNT 4

#define BUTTON0_NODE  DT_ALIAS(button0)
#if !DT_NODE_HAS_STATUS(BUTTON0_NODE, okay)
#error "Unsupported board: button0 devicetree alias is not defined"
#endif

zigbee_state_e zigbee_state = ZIGBEE_STEERING;

extern "C" {
#include <addons/zcl/zb_zcl_temp_measurement_addons.h>
#include "zb_plant_sensor.h"
#include "zb_zcl_power_config.h"
#include "zboss_api_zdo.h"


  static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(BUTTON0_NODE, gpios,
      {0});
  static struct gpio_callback button_cb_data;

  const struct gpio_dt_spec led_spec =  GPIO_DT_SPEC_GET(LED0_NODE, gpios);
  static uint8_t steering_failed_counter = 0;

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

  /* Forward declaration of the buttun pressed function */
  void button_pressed(const struct device *dev, struct gpio_callback *cb,
      uint32_t pins);

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
  void factory_reset_timer_expiry(struct k_timer *timer_id);

  K_TIMER_DEFINE(led_blink_timer, led_blink_timer_cb, NULL);
  K_TIMER_DEFINE(factory_reset_timer, factory_reset_timer_expiry, NULL);


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
    
    if(sig != ZB_COMMON_SIGNAL_CAN_SLEEP){
      LOG_DBG("SIGNAL: %d", sig);
    }
    switch (sig) {
      case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        // Fall through
      case ZB_BDB_SIGNAL_STEERING:
        if(status == RET_OK){
          zigbee_state = ZIGBEE_JOINING;
          steering_failed_counter = 0;
          LOG_DBG("Steering success");
          ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
        }
        else{
          // Start the error blink
          zigbee_state = ZIGBEE_STEERING;
          steering_failed_counter += 1;
          k_timer_stop(&led_blink_timer);
          k_timer_start(&led_blink_timer, K_MSEC(100), K_MSEC(100));
          // Do not continue searching for a network after a few failed attempts 
          if(steering_failed_counter <= MAX_STEERING_COUNT){
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
          }
          else{
            LOG_INF("Failed steering after %d attempts", MAX_STEERING_COUNT);
            gpio_pin_set_dt(&led_spec, 0);
            k_timer_stop(&led_blink_timer);
            zigbee_state = ZIGBEE_ERROR;
            zb_sleep_now();
          }
        }
        break;
      case ZB_ZDO_SIGNAL_LEAVE: {
        /* Update network status LED */
        if(status == RET_OK){
          zb_zdo_signal_leave_params_t *leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(&sig, zb_zdo_signal_leave_params_t);
          LOG_INF("Network left (leave type: %d)", leave_params->leave_type);

          /* Set joining_signal_received to false so broken rejoin procedure can be detected correctly. */
          if (leave_params->leave_type == ZB_NWK_LEAVE_TYPE_REJOIN) {
              zigbee_state == ZIGBEE_ERROR;
          }
        }
        LOG_DBG("Left network");
        zigbee_state = ZIGBEE_STEERING;
        gpio_pin_set_dt(&led_spec, 1);
        ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
        break;
      } 
      case ZB_NLME_STATUS_INDICATION: {
        zb_zdo_signal_nlme_status_indication_params_t *nlme_status_ind =
          ZB_ZDO_SIGNAL_GET_PARAMS(&sig, zb_zdo_signal_nlme_status_indication_params_t);
        if (nlme_status_ind->nlme_status.status == ZB_NWK_COMMAND_STATUS_PARENT_LINK_FAILURE) {

          /* Check for broken rejoin procedure and restart the device to recover. */
          if (zigbee_state == ZIGBEE_ERROR) {
              zb_reset(0);
          }
        }
        break;
      }

      case ZB_COMMON_SIGNAL_CAN_SLEEP:
        if(zigbee_state == ZIGBEE_JOINING){ // Joining success
          // Reduce the data polling to save power
          LOG_DBG("Joined network");
          zb_zdo_pim_set_long_poll_interval(CONFIG_ZIGBEE_DATA_POLL_INTERVAL);
          k_timer_stop(&led_blink_timer);
          gpio_pin_set_dt(&led_spec, 0);
          zigbee_state = ZIGBEE_JOINED;
        }
        zb_sleep_now();
        break;

      default:
        ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
        break;
    }


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

  void button_init(){
    int ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
      LOG_ERR("Error %d: failed to configure %s pin %d\n",
          ret, button.port->name, button.pin);
      return;
    }

    ret = gpio_pin_interrupt_configure_dt(&button,
        GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
      LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n",
          ret, button.port->name, button.pin);
      return;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
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


  /// @brief Butten pressed callback
  ///
  /// @param dev device beloning to the button that is pressed
  /// @param cb callback context
  /// @param pins
  void button_pressed(const struct device *dev, struct gpio_callback *cb,
      uint32_t pins)
  {
    // Stop the timer if one is already running
    LOG_DBG("Button Pressed");
    k_timer_stop(&factory_reset_timer);
    k_timer_start(&factory_reset_timer, K_MSEC(FACTORY_RESET_TIMEOUT), K_NO_WAIT);

  }

  void factory_reset_timer_expiry(struct k_timer *timer_id){
    int32_t button_state = gpio_pin_get_dt(&button);
    if(button_state){
      // Do a factory reset
      k_timer_stop(&led_blink_timer);
      LOG_INF("Schedule Factory Reset; stop timer; set factory_reset_done flag");
      ZB_SCHEDULE_APP_CALLBACK(zb_bdb_reset_via_local_action, 0);
    }
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
  button_init();

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
