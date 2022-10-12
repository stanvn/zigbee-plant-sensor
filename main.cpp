/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "logging/log.h"
#include "buildin-led.hpp"

LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

void main(void)
{
  LOG_INF("Application started!");
  buildin_led_init();
  while (true)
  {
    buildin_led_toggle();
    k_msleep(1000);
  }
}
