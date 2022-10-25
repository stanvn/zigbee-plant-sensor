extern "C" {
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_zcl_soil_moisture.h"

  // Most durty hack to overcome one of the zboss zcl bugs
#define bat_num

#define ZCL_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
#define ZCL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
#define ZCL_SOIL_MOISTURE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
#define ZCL_ILLUMIANCE_MEASUREMENT_MEASURED_VALUE_MULTIPLIER 100
#define ZCL_BATTERY_VOLTAGE_VALUE_DIVIDER 100
 


#define PLANT_SENSOR_ENDPOINT     0x01

#define ZB_DEVICE_VER_PLANT_SENSOR         1                                    /**< Light Sensor device version. */

#define ZB_PLANT_SENSOR_IN_CLUSTER_NUM     8                                    /**< Number of the input (server) clusters in the light sensor device. */

#define ZB_PLANT_SENSOR_OUT_CLUSTER_NUM    1

#define ZB_PLANT_SENSOR_REPORT_ATTR_COUNT  8
  /** @brief Declares simple descriptor for the "Device_name" device.
   *
   *  @param ep_name          Endpoint variable name.
   *  @param ep_id            Endpoint ID.
   *  @param in_clust_num     Number of the supported input clusters.
   *  @param out_clust_num    Number of the supported output clusters.
   */

  /// @brief  Humidity Measurement cluster attributes
  typedef struct{
    zb_int16_t measure_value;
    zb_int16_t min_measure_value;
    zb_int16_t max_measure_value;
  } zb_zcl_rel_humidity_measurement_attr_t;

  typedef struct
  {
    zb_uint8_t battery_voltage;
    zb_uint8_t battery_size;
    zb_uint8_t battery_quantity;
    zb_uint8_t battery_rated_voltage;
    zb_uint8_t battery_alarm_mask;
    zb_uint8_t battery_voltage_min_threshold;
    zb_uint8_t battery_percentage_remaining;
    zb_uint8_t battery_voltage_threshold1;
    zb_uint8_t battery_voltage_threshold2;
    zb_uint8_t battery_voltage_threshold3;
    zb_uint8_t battery_percentage_min_threshold;
    zb_uint8_t battery_percentage_threshold1;
    zb_uint8_t battery_percentage_threshold2;
    zb_uint8_t battery_percentage_threshold3;
    zb_uint32_t battery_alarm_state;

  } zb_zcl_power_config_attr_t;

  typedef struct zb_zcl_illuminance_measurement_attr {
    zb_int16_t measure_value;
    zb_int16_t min_measure_value;
    zb_int16_t max_measure_value;
  } zb_zcl_illuminance_measurement_attr_t;

#define ZB_DECLARE_PLANT_SENSOR_CLUSTER_LIST(                            \
    cluster_list_name,                                                   \
    basic_attr_list,                                                     \
    identify_attr_list,                                                  \
    temperature_measure_attr_list,                                       \
    humidity_measure_attr_list,                                          \
    moisture_measure_attr_list,                                          \
    illumiance_measure_attr_list,                                        \
    power_config_attr_list)                                              \
  zb_zcl_cluster_desc_t cluster_list_name[] =                            \
  {                                                                      \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_BASIC,                                         \
        ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),               \
        (basic_attr_list),                                               \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                      \
        ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),            \
        (identify_attr_list),                                            \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT ,                             \
        ZB_ZCL_ARRAY_SIZE(temperature_measure_attr_list, zb_zcl_attr_t), \
        (temperature_measure_attr_list),                                 \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,                      \
        ZB_ZCL_ARRAY_SIZE(humidity_measure_attr_list, zb_zcl_attr_t),    \
        (humidity_measure_attr_list),                                \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_SOIL_MOISTURE_MEASUREMENT,                     \
        ZB_ZCL_ARRAY_SIZE(moisture_measure_attr_list, zb_zcl_attr_t),    \
        (moisture_measure_attr_list),                                    \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,                       \
        ZB_ZCL_ARRAY_SIZE(illumiance_measure_attr_list, zb_zcl_attr_t),  \
        (illumiance_measure_attr_list),                                  \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                  \
        ZB_ZCL_ARRAY_SIZE(power_config_attr_list, zb_zcl_attr_t),        \
        (power_config_attr_list),                                        \
        ZB_ZCL_CLUSTER_SERVER_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        ),                                                               \
    ZB_ZCL_CLUSTER_DESC(                                                 \
        ZB_ZCL_CLUSTER_ID_IDENTIFY,                                      \
        0,                                                               \
        NULL,                                                            \
        ZB_ZCL_CLUSTER_CLIENT_ROLE,                                      \
        ZB_ZCL_MANUF_CODE_INVALID                                        \
        )                                                                \
  }

#define ZB_ZCL_DECLARE_PLANT_SENSOR_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num) simple_desc_##ep_name =         \
  {                                                                                   \
    ep_id,                                                                            \
    ZB_AF_HA_PROFILE_ID,                                                              \
    ZB_HA_CUSTOM_ATTR_DEVICE_ID,                                                      \
    ZB_DEVICE_VER_PLANT_SENSOR,                                                 \
    0,                                                                                \
    in_clust_num,                                                                     \
    out_clust_num,                                                                    \
    {                                                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
      ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,                                             \
      ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT,                                     \
      ZB_ZCL_CLUSTER_ID_SOIL_MOISTURE_MEASUREMENT,                                    \
      ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT,                                      \
      ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                                 \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
    }                                                                                 \
  }

  /** @brief Declares endpoint for the temperature sensor device.
   *
   *  @param ep_name          Endpoint variable name.
   *  @param ep_id            Endpoint ID.
   *  @param cluster_list     Endpoint cluster list.
   */
#define ZB_ZCL_DECLARE_PLANT_SENSOR_EP(ep_name, ep_id, cluster_list)              \
  ZB_ZCL_DECLARE_PLANT_SENSOR_DESC(                                               \
      ep_name,                                                                    \
      ep_id,                                                                      \
      ZB_PLANT_SENSOR_IN_CLUSTER_NUM,                                             \
      ZB_PLANT_SENSOR_OUT_CLUSTER_NUM);                                           \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(                                             \
      reporting_info##ep_name,                                                    \
      ZB_PLANT_SENSOR_REPORT_ATTR_COUNT);                                         \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                                    \
      ep_name,                                                                    \
      ep_id,                                                                      \
      ZB_AF_HA_PROFILE_ID,                                                        \
      0,                                                                          \
      NULL,                                                                       \
      ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                     \
      cluster_list,                                                               \
      (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                           \
      ZB_PLANT_SENSOR_REPORT_ATTR_COUNT, reporting_info##ep_name, 0, NULL)
}

