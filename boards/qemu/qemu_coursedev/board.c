/*
 * From-scratch board initialization for qemu_coursedev (L5 task 2).
 *
 * board_late_init_hook() is registered by selecting BOARD_LATE_INIT_HOOK
 * in Kconfig. The kernel invokes it after POST_KERNEL initialization but
 * before boot_banner() and before the application's main() entry point,
 * so the message appears ahead of the boot banner and main().
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/platform/hooks.h>

void board_late_init_hook(void)
{
	printk("Board Initialized\n");
}
