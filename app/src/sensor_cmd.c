/*
 * L7-task1: custom shell root command "sensorroot" with the subcommands:
 *
 *   sensorroot fetch   -> sensor_sample_fetch()  (LED ON)
 *   sensorroot read    -> sensor_channel_get()   (LED OFF, prints value)
 *   sensorroot info    -> prints sensor device information
 *
 * L7-task2: adds the subcommand
 *
 *   sensorroot set <value> -> led_sensor_set_brightness() (extension API)
 *
 * whose argument is validated with SHELL_CMD_ARG + shell_error().
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include <errno.h>
#include <stdlib.h>

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

/*
 * L7-task2: "sensorroot set <value>".
 *
 * Exposes the L6-task2 extension API (led_sensor_set_brightness()) as a
 * shell subcommand. Argument validation:
 *  - SHELL_CMD_ARG(..., 2, 0) makes the shell itself reject the command
 *    when the <value> argument is missing;
 *  - the value format and range are checked here and reported with
 *    shell_error().
 */
static int cmd_set(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *sensor = led_sensor_get_device();
	char *endptr;
	long value;
	int ret;

	ARG_UNUSED(argc);

	/* SHELL_CMD_ARG guarantees argv[1] exists; keep a defensive check. */
	if (argc < 2 || argv[1] == NULL) {
		shell_error(sh, "usage: sensorroot set <value> (0-100)");
		return -EINVAL;
	}

	errno = 0;
	value = strtol(argv[1], &endptr, 10);
	if (errno != 0 || endptr == argv[1] || *endptr != '\0') {
		shell_error(sh, "invalid value '%s': not a number", argv[1]);
		shell_print(sh, "usage: sensorroot set <value> (0-100)");
		return -EINVAL;
	}

	if (value < 0 || value > 100) {
		shell_error(sh, "value %ld out of range: must be 0-100", value);
		return -EINVAL;
	}

	ret = led_sensor_set_brightness(sensor, (uint8_t)value);
	if (ret != 0) {
		shell_error(sh, "led_sensor_set_brightness failed (%d)", ret);
		return ret;
	}

	shell_print(sh, "brightness set to %ld", value);

	return 0;
}

/* Subcommand set of the "sensorroot" root command. */
SHELL_STATIC_SUBCMD_SET_CREATE(sensorroot_subs,
	SHELL_CMD_ARG(fetch, NULL, "Fetch a sensor sample (LED ON)", cmd_fetch, 1, 0),
	SHELL_CMD_ARG(read, NULL, "Read the sensor sample (LED OFF)", cmd_read, 1, 0),
	SHELL_CMD_ARG(info, NULL, "Show sensor device information", cmd_info, 1, 0),
	/* L7-task2: mandatory <value> argument (mandatory 1, optional 0). */
	SHELL_CMD_ARG(set, NULL, "Set LED brightness: set <value> (0-100)", cmd_set, 2, 0),
	SHELL_SUBCMD_SET_END);

/* Root command: "sensorroot". */
SHELL_CMD_ARG_REGISTER(sensorroot, &sensorroot_subs, "LED sensor root command", NULL, 1, 0);
