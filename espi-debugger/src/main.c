#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
LOG_MODULE_REGISTER(espi_debugger,LOG_LEVEL_DBG);

static const struct device *const espi_dev = DEVICE_DT_GET(DT_NODELABEL(espi0));

int main(void)
{
	#ifdef CONFIG_LOG
	LOG_INF("Hello from Main()\n");
	#endif

	/* Indicate to eSPI controller (host) simplest configuration: Single line,
	 * 20MHz frequency and only logical channel 0 and 1 are supported
	 */
	struct espi_cfg cfg = {
		.io_caps = ESPI_IO_MODE_SINGLE_LINE,
		.channel_caps = ESPI_CHANNEL_PERIPHERAL | ESPI_CHANNEL_VWIRE,
		.max_freq = 20,
	};

	/*Init eSPI device*/
	if (!device_is_ready(espi_dev)) {
		#ifdef CONFIG_LOG
		LOG_ERR("%s: eSPI device not ready.", espi_dev->name);
		#endif
		return -ENODEV;
	}

	int ret = espi_config(espi_dev, &cfg);
	if (ret != 0) {
		#ifdef CONFIG_LOG
		LOG_ERR("Failed to configure eSPI target channels:%x err: %d", cfg.channel_caps, ret);
		#endif
		return ret;
	} else {
		#ifdef CONFIG_LOG
		LOG_INF("eSPI target configured successfully!");
		#endif
	}

	/*
	 * Nothing copies UART bytes in software. When the host enables the eSPI
	 * Peripheral Channel, espi_mchp_xec_host_v2.c programs the UART1 I/O BAR
	 * and Serial IRQ. Host UART accesses then reach the UART1 hardware.
	 */
	for (;;) {
		k_sleep(K_FOREVER);
	}
}