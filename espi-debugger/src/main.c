#include <stdio.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <soc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log.h>

static const struct device *const espi_dev = DEVICE_DT_GET(DT_NODELABEL(espi0));
static struct espi_callback p80_cb;

#define ESPI_FREQ_20MHZ 20u
#define ESPI_FREQ_25MHZ 25u
#define ESPI_FREQ_66MHZ 66u

#define K_WAIT_DELAY 100u

/* eSPI event */
#define EVENT_MASK         0x0000FFFFu
#define EVENT_DETAILS_MASK 0xFFFF0000u
#define EVENT_DETAILS_POS  16u
#define EVENT_TYPE(x)      (x & EVENT_MASK)
#define EVENT_DETAILS(x)   ((x & EVENT_DETAILS_MASK) >> EVENT_DETAILS_POS)

#define PWR_SEQ_TIMEOUT 3000u

LOG_MODULE_DECLARE(espi, CONFIG_ESPI_LOG_LEVEL);

/* eSPI peripheral channel notifications handler */
static void periph_handler(const struct device *dev, struct espi_callback *cb,
			   struct espi_event event)
{
	uint8_t periph_type;
	uint8_t periph_index;

	periph_type = EVENT_TYPE(event.evt_details);
	periph_index = EVENT_DETAILS(event.evt_details);

	switch (periph_type) {
	case ESPI_PERIPHERAL_DEBUG_PORT80:
		LOG_INF("Postcode %x", event.evt_data);
		break;
	case ESPI_PERIPHERAL_UART:
		LOG_INF("Uart: %x", event.evt_data);
		break;
	default:
		LOG_INF("%s periph 0x%x [%x]", __func__, periph_type, event.evt_data);
	}
}


int espi_init(void)
{	
	int ret;
	/* Indicate to eSPI controller (host) simplest configuration: Single line,
	 * 20MHz frequency and only logical channel 0 and 1 are supported
	 */

	struct espi_cfg cfg = {
		.io_caps = ESPI_IO_MODE_SINGLE_LINE,
		.channel_caps = ESPI_CHANNEL_PERIPHERAL,
		.max_freq = ESPI_FREQ_20MHZ,
	};

	/*Init eSPI callbacks*/
	ret = espi_config(espi_dev, &cfg);
	if (ret) {
		LOG_ERR("Failed to configure eSPI target channels:%x err: %d", cfg.channel_caps,
			ret);
		return ret;
	} else {
		LOG_INF("eSPI target configured successfully!");
	}

	LOG_INF("eSPI test - callbacks initialization... ");
	espi_init_callback(&p80_cb, periph_handler, ESPI_BUS_PERIPHERAL_NOTIFICATION);
	LOG_INF("complete");

	LOG_INF("eSPI test - callbacks registration... ");
	espi_add_callback(espi_dev, &p80_cb);

	LOG_INF("complete");
	return ret;
}



int main(void)
{
	printf("Hello from Main()\n");

	/*Init eSPI device*/
	if (!device_is_ready(espi_dev)) {
		LOG_ERR("%s: eSPI device not ready.", espi_dev->name);
		return -ENODEV;
	}

	espi_init();


	return 0;
}
