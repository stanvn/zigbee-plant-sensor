#include "buildin-led.hpp"

#include <device.h>
#include <drivers/gpio.h>

#include  <logging/log.h>
LOG_MODULE_REGISTER(buildin_led);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN		DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif

bool led_is_on = true;
const struct device *dev;

void buildin_led_init(){
	int ret;

	dev = device_get_binding(LED0);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}
}

void buildin_led_toggle(){
  LOG_DBG("Toggle LED %s", dev->name);
  gpio_pin_set(dev, PIN, (int)led_is_on);
  led_is_on = !led_is_on;
}
