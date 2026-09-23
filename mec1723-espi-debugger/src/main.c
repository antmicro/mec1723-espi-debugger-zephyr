#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <drivers/auxdisplay.h>

LOG_MODULE_REGISTER(espi_debugger,LOG_LEVEL_DBG);

const struct device *auxdisplay_h =	DEVICE_DT_GET(DT_NODELABEL(auxdisplay_h));

const struct device *auxdisplay_l =	DEVICE_DT_GET(DT_NODELABEL(auxdisplay_l));

static const struct device *const espi_dev = DEVICE_DT_GET(DT_NODELABEL(espi0));

static struct espi_callback p80_cb;

static void p80(const struct device *dev,struct espi_callback *cb,struct espi_event e)
{
    if ((e.evt_details & 0xffff) == ESPI_PERIPHERAL_DEBUG_PORT80) {
        
		uint8_t p80_code = (uint8_t)e.evt_data;
		printk("POST: %02x\n",p80_code);

		uint8_t high = (e.evt_data >> 4) & 0x0F;
		uint8_t low = e.evt_data & 0x0F;
		LOG_DBG("To-7-seg: %1x %1x\n",high,low);

		const char hex[] = "0123456789ABCDEF";
		high = hex[high];
		low = hex[low];

		LOG_DBG("To-7-seg-char: %1x %1x\n",high,low);

		auxdisplay_clear(auxdisplay_h);
		auxdisplay_clear(auxdisplay_l);

		auxdisplay_write(auxdisplay_h, &high, 1);
		auxdisplay_write(auxdisplay_l, &low, 1);
    }
}

int main(void)
{
	
	LOG_INF("Hello from Main()\n");

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
		LOG_ERR("%s: eSPI device not ready.", espi_dev->name);
		return -ENODEV;
	}

	if (!device_is_ready(auxdisplay_h) || !device_is_ready(auxdisplay_l)) {
		LOG_ERR("Failed to init 7seg display");
		return 0;
	}

	int rc = auxdisplay_cursor_set_enabled(auxdisplay_h, true);

	if (rc != 0) {
		LOG_ERR("Failed to enable cursor on %s: %d",auxdisplay_h->name, rc);
	}

	rc = auxdisplay_cursor_set_enabled(auxdisplay_l, true);

	if (rc != 0) {
		LOG_ERR("Failed to enable cursor on %s: %d",auxdisplay_l->name, rc);
	}

	int ret = espi_config(espi_dev, &cfg);
	if (ret != 0) {
		LOG_ERR("Failed to configure eSPI target channels:%x err: %d", cfg.channel_caps, ret);
	
		return ret;
	} else {
		LOG_INF("eSPI target configured successfully!");
	}

	// Set 7seg to initial value of 0xFF
	LOG_INF("set init 7seg to 0xFF");
	auxdisplay_clear(auxdisplay_h);
	auxdisplay_clear(auxdisplay_l);
	auxdisplay_write(auxdisplay_h, (const uint8_t *)"F", 1);
	auxdisplay_write(auxdisplay_l, (const uint8_t *)"F", 1);
	
	// Create P80 callback to display it onto 7seg display
	espi_init_callback(&p80_cb, p80, ESPI_BUS_PERIPHERAL_NOTIFICATION);
	espi_add_callback(espi_dev, &p80_cb);

	/*
	 * Nothing copies UART bytes in software. When the host enables the eSPI
	 * Peripheral Channel, espi_mchp_xec_host_v2.c programs the UART1 I/O BAR
	 * and Serial IRQ. Host UART accesses then reach the UART1 hardware.
	 */
	for (;;) {
		k_sleep(K_FOREVER);
	}
}