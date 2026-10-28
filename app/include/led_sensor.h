/*
 * L6-task1: LED-based custom sensor driver.
 *
 * The driver implements the Zephyr sensor API:
 *   - sensor_sample_fetch(): turns the LED ON
 *   - sensor_channel_get(): turns the LED OFF and returns the sample value
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

#ifdef __cplusplus
}
#endif

#endif /* COURSE_LED_SENSOR_H_ */
