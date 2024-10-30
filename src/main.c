#include "lcd_screen_i2c.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>





#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_SCREEN_NODE DT_ALIAS(lcd_screen)


/* Data of ADC io-channels specified in devicetree. */
#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

static const struct i2c_dt_spec lcd_screen_dev = I2C_DT_SPEC_GET(LCD_SCREEN_NODE);
const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);

struct sensor_value temperature;
struct sensor_value humidity;

// pour l'ADC : 
	uint16_t buf;
	
	struct adc_sequence sequence = {
	.buffer = &buf,
	/* buffer size in bytes, not number of samples */
	.buffer_size = sizeof(buf), };








//##############################################################################################
// MAIN #########################################################################################

int main() {

	// Init GPIO
	gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);

    // Init device
    init_lcd(&lcd_screen_dev);

    write_lcd(&lcd_screen_dev, HELLO_MSG, LCD_LINE_1);
    //write_lcd_clear(&dev_lcd_screen, ZEPHYR_MSG LCD_LINE_2);

	// Récupération de la température et de l'humidité par le dht11

	int temperature_= 0;
	int humidite_= 0;
	


	

	while(1){


		printk("ADC reading[%u]:\n");
		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			int32_t val_mv;

			printk("- %s, channel %d: ",
			       adc_channels[i].dev->name,
			       adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

		
			/*
			 * If using differential mode, the 16 bit value
			 * in the ADC sample buffer should be a signed 2's
			 * complement value.
			 */
			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}
			printk("%"PRId32, val_mv);
			
			/* conversion to mV may not be supported, skip if not */

				printk(" = %"PRId32" mV\n", val_mv);
			}
		}

		
	sensor_sample_fetch(dht11);
	sensor_channel_get(dht11, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
	sensor_channel_get(dht11, SENSOR_CHAN_HUMIDITY, &humidity);

	temperature_ = sensor_value_to_double(&temperature);
	humidite_ = sensor_value_to_double(&humidity);

	printf("RELEVE: \n");
	printf("temperature = %d", temperature_);
	printf("\n");
	printf("humidite = %d", humidite_);
	printf("\n");
	
	k_sleep(K_SECONDS(5));

	}

