/*
 * L6-task1: use the custom LED sensor driver from main.
 *
 * sensor_sample_fetch() turns the LED ON, sensor_channel_get() turns it OFF.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#include "led_sensor.h"

/* LED alias defined in app.overlay */
#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int main(void)
{
	const struct device *sensor = led_sensor_get_device();
	struct sensor_value val;
	int ret;

	if (!device_is_ready(sensor)) {
		printk("LED sensor device is not ready\n");
		return -ENODEV;
	}

	printk("Hello World! %s\n", CONFIG_BOARD);
	printk("LED sensor driver: %s\n", sensor->name);

	/* L6-task2: call the custom extension API from main.
	 * It changes the "brightness" parameter in the driver's
	 * dynamic data structure (struct led_sensor_data).
	 */
	uint8_t brightness;

	ret = led_sensor_set_brightness(sensor, 50);
	if (ret != 0) {
		printk("led_sensor_set_brightness failed: %d\n", ret);
		return ret;
	}

	led_sensor_get_brightness(sensor, &brightness);
	printk("LED brightness after extension API call: %u\n", brightness);

	while (1) {
		/* Fetch a sample -> LED ON */
		ret = sensor_sample_fetch(sensor);
		if (ret != 0) {
			printk("sensor_sample_fetch failed: %d\n", ret);
			return ret;
		}

		printk("sample fetched (LED ON)\n");

		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS / 2);

		/* Read the sample -> LED OFF */
		ret = sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &val);
		if (ret != 0) {
			printk("sensor_channel_get failed: %d\n", ret);
			return ret;
		}

		printk("sample read: %lld (LED OFF)\n", val.val1);

		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS / 2);
	}

	return 0;
}
