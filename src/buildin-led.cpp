#include "buildin-led.hpp"

#include <device.h>
#include <drivers/gpio.h>

#include  <logging/log.h>
LOG_MODULE_REGISTER(buildin_led);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

bool led_is_on = true;
const struct gpio_dt_spec led_spec =  GPIO_DT_SPEC_GET(LED0_NODE, gpios);
void buildin_led_init(){
	int ret;

	if (!device_is_ready(led_spec.port)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led_spec, GPIO_OUTPUT);
	if (ret < 0) {
		return;
	}
}

void buildin_led_toggle(){
  LOG_DBG("Toggle LED");
  gpio_pin_set_dt(&led_spec, (int)led_is_on);
  led_is_on = !led_is_on;
}


