/*
 * L6-task1: LED-based custom sensor driver.
 *
 * Implements the sensor driver API:
 *   - sensor_sample_fetch() : sets the LED state to ON
 *   - sensor_channel_get()  : sets the LED state to OFF and returns a value
 *
 * L6-task2: custom extension API (led_sensor_set_brightness()) that changes
 * the "brightness" parameter stored in the dynamic data structure.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "led_sensor.h"

LOG_MODULE_REGISTER(led_sensor, CONFIG_LOG_DEFAULT_LEVEL);

/* The LED is taken from the devicetree alias "app-led" (see app.overlay). */
#define LED_SENSOR_NODE DT_ALIAS(app_led)

/* Dynamic data structure of the driver instance (device->data). */
struct led_sensor_data {
	bool led_on;
	uint32_t sample_count;
	uint8_t brightness; /* 0..100, changed by the extension API */
};

static int led_sensor_init(const struct device *dev)
{
	const struct gpio_dt_spec *led = dev->config;
	int ret;

	if (!gpio_is_ready_dt(led)) {
		LOG_ERR("LED device not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("LED configure failed (%d)", ret);
		return ret;
	}

	struct led_sensor_data *data = dev->data;

	data->led_on = true;
	data->sample_count = 0U;
	data->brightness = 100U;

	LOG_DBG("LED sensor driver initialized");
	return 0;
}

/* Sensor API: a fetch turns the LED ON. */
static int led_sensor_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct gpio_dt_spec *led = dev->config;
	struct led_sensor_data *data = dev->data;
	int ret;

	ret = gpio_pin_set_dt(led, 1);
	if (ret < 0) {
		return ret;
	}

	data->led_on = true;
	data->sample_count++;

	return 0;
}

/* Sensor API: reading a channel turns the LED OFF. */
static int led_sensor_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct gpio_dt_spec *led = dev->config;
	struct led_sensor_data *data = dev->data;
	int ret;

	ret = gpio_pin_set_dt(led, 0);
	if (ret < 0) {
		return ret;
	}

	data->led_on = false;

	/* Return the number of samples taken so far as the "reading". */
	val->val1 = (int64_t)data->sample_count;
	val->val2 = 0;

	return 0;
}

static const struct sensor_driver_api led_sensor_api = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get = led_sensor_channel_get,
};

static const struct gpio_dt_spec led_spec = GPIO_DT_SPEC_GET(LED_SENSOR_NODE, gpios);

static struct led_sensor_data led_sensor_data;

DEVICE_DEFINE(led_sensor, "LED Sensor", led_sensor_init, NULL, &led_sensor_data,
	      &led_spec, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &led_sensor_api);

const struct device *led_sensor_get_device(void)
{
	return DEVICE_GET(led_sensor);
}

/*
 * L6-task2: custom extension API.
 *
 * Not part of struct sensor_driver_api: it is a project-specific function
 * that changes the "brightness" parameter in the driver's dynamic data
 * structure (struct led_sensor_data, referenced by device->data).
 */
int led_sensor_set_brightness(const struct device *dev, uint8_t brightness)
{
	struct led_sensor_data *data;

	if (dev == NULL) {
		return -ENODEV;
	}

	if (brightness > 100U) {
		return -EINVAL;
	}

	data = dev->data;
	data->brightness = brightness;

	LOG_DBG("brightness set to %u", data->brightness);
	return 0;
}

int led_sensor_get_brightness(const struct device *dev, uint8_t *brightness)
{
	struct led_sensor_data *data;

	if (dev == NULL || brightness == NULL) {
		return -ENODEV;
	}

	data = dev->data;
	*brightness = data->brightness;

	return 0;
}
