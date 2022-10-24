#pragma once
#include "status_codes.hpp"

extern "C" {
#include <zboss_api.h>
#include <zboss_api_addons.h>
#include <zigbee/zigbee_error_handler.h>
#include <zigbee/zigbee_app_utils.h>
#include <zb_nrf_platform.h>
}

/* Version of the application software (1 byte). */
#define APP_INIT_BASIC_APP_VERSION     01

/* Version of the implementation of the Zigbee stack (1 byte). */
#define APP_INIT_BASIC_STACK_VERSION   10

/* Version of the hardware of the device (1 byte). */
#define APP_INIT_BASIC_HW_VERSION      11

/* Manufacturer name (32 bytes). */
#define APP_INIT_BASIC_MANUF_NAME      "Custom devices (DiY)"

/* Model number assigned by manufacturer (32-bytes long string). */
#define APP_INIT_BASIC_MODEL_ID        "plant_sensor"

/* First 8 bytes specify the date of manufacturer of the device
 * in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific.
 */
#define APP_INIT_BASIC_DATE_CODE       "20200329"

/* Type of power sources available for the device.
 * For possible values see section 3.2.2.2.8 of ZCL specification.
 */
#define APP_INIT_BASIC_POWER_SOURCE    ZB_ZCL_BASIC_POWER_SOURCE_BATTERY

/* Describes the physical location of the device (16 bytes).
 * May be modified during commissioning process.
 */
#define APP_INIT_BASIC_LOCATION_DESC   "Office desk"

/* Describes the type of physical environment.
 * For possible values see section 3.2.2.2.10 of ZCL specification.
 */
#define APP_INIT_BASIC_PH_ENV          ZB_ZCL_BASIC_ENV_UNSPECIFIED

/* Device endpoint, used to receive ZCL commands. */
#define TEMPERATURE_SENSOR_ENDPOINT     0x01
#define WATER_CONTENT_ENDPOINT            0x02


status_code_t zigbee_start();

