/*
 * L6-task1/L6-task2: LED-based custom sensor driver.
 *
 * The driver implements the Zephyr sensor API:
 *   - sensor_sample_fetch(): turns the LED ON
 *   - sensor_channel_get(): turns the LED OFF and returns the sample value
 *
 * L6-task2 adds a custom extension API (outside the sensor_driver_api):
 *   - led_sensor_set_brightness(): changes the "brightness" parameter stored
 *     in the driver's dynamic (device) data structure
 */

#ifndef COURSE_LED_SENSOR_H_
#define COURSE_LED_SENSOR_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the device pointer of the custom LED sensor driver.
 *
 * The driver instance is defined with DEVICE_DEFINE() in led_sensor.c.
 */
const struct device *led_sensor_get_device(void);

/**
 * L6-task2: custom extension API.
 *
 * Changes the "brightness" parameter kept in the driver's dynamic data
 * structure (struct led_sensor_data). This is a project-specific API,
 * independent of the standard sensor_driver_api.
 *
 * @param dev        LED sensor device
 * @param brightness new brightness value (0..100)
 *
 * @return 0 on success, -EINVAL on invalid value, -ENODEV if dev is NULL
 */
int led_sensor_set_brightness(const struct device *dev, uint8_t brightness);

/**
 * L6-task2: custom extension API counterpart.
 *
 * Reads the "brightness" parameter from the driver's dynamic data structure.
 *
 * @param dev        LED sensor device
 * @param brightness output location for the current brightness (0..100)
 *
 * @return 0 on success, -ENODEV/-EINVAL on error
 */
int led_sensor_get_brightness(const struct device *dev, uint8_t *brightness);

#ifdef __cplusplus
}
#endif

#endif /* COURSE_LED_SENSOR_H_ */
