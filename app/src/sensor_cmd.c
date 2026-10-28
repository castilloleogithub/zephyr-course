/*
 * L7-task1: custom shell root command "sensorroot" with the subcommands:
 *
 *   sensorroot fetch   -> sensor_sample_fetch()  (LED ON)
 *   sensorroot read    -> sensor_channel_get()   (LED OFF, prints value)
 *   sensorroot info    -> prints sensor device information
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include "led_sensor.h"

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *sensor = led_sensor_get_device();
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(sensor)) {
		shell_error(sh, "LED sensor device is not ready");
		return -ENODEV;
	}

	ret = sensor_sample_fetch(sensor);
	if (ret != 0) {
		shell_error(sh, "sensor_sample_fetch failed (%d)", ret);
		return ret;
	}

	shell_print(sh, "sample fetched -> LED ON");

	return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *sensor = led_sensor_get_device();
	struct sensor_value val;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(sensor)) {
		shell_error(sh, "LED sensor device is not ready");
		return -ENODEV;
	}

	ret = sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &val);
	if (ret != 0) {
		shell_error(sh, "sensor_channel_get failed (%d)", ret);
		return ret;
	}

	shell_print(sh, "sample read: %d -> LED OFF", (int)val.val1);

	return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *sensor = led_sensor_get_device();

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "device     : %s", sensor->name);
	shell_print(sh, "ready      : %s", device_is_ready(sensor) ? "yes" : "no");
	shell_print(sh, "driver api : %p", (const void *)sensor->api);

	return 0;
}

/* Subcommand set of the "sensorroot" root command. */
SHELL_STATIC_SUBCMD_SET_CREATE(sensorroot_subs,
	SHELL_CMD_ARG(fetch, NULL, "Fetch a sensor sample (LED ON)", cmd_fetch, 1, 0),
	SHELL_CMD_ARG(read, NULL, "Read the sensor sample (LED OFF)", cmd_read, 1, 0),
	SHELL_CMD_ARG(info, NULL, "Show sensor device information", cmd_info, 1, 0),
	SHELL_SUBCMD_SET_END);

/* Root command: "sensorroot". */
SHELL_CMD_ARG_REGISTER(sensorroot, &sensorroot_subs, "LED sensor root command", NULL, 1, 0);
