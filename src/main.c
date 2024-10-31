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
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/drivers/gpio.h>

#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_SCREEN_NODE DT_ALIAS(lcd_screen)
#define BOUTON_GAUCHE_NODE DT_ALIAS(bouton_gauche)
#define BOUTON_DROITE_NODE DT_ALIAS(bouton_droite)

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

static const struct i2c_dt_spec lcd_screen_dev = I2C_DT_SPEC_GET(LCD_SCREEN_NODE);
const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);
static const struct adc_dt_spec adc_channels[] = {DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)};
const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
static struct gpio_dt_spec bouton_G = GPIO_DT_SPEC_GET_OR(BOUTON_GAUCHE_NODE, gpios, {0});
static struct gpio_dt_spec bouton_D = GPIO_DT_SPEC_GET_OR(BOUTON_DROITE_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

struct sensor_value temperature;
struct sensor_value humidity;


volatile bool flag_etat = 0; 

// FONCTIONS 
void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());

	flag_etat = 1;

}


// ##############################################################################################
//  MAIN #########################################################################################

int main()
{
	

	// Initialisation des GPIOs
		gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);
		gpio_pin_configure_dt(&bouton_G, GPIO_INPUT);
		gpio_pin_configure_dt(&bouton_D, GPIO_INPUT);

		gpio_init_callback(&button_cb_data, button_pressed, 16);

	// Initialisation périphériques
		init_lcd(&lcd_screen_dev);

	// affichage de messages sur l'écran LCD, par appel des MACRO définits dans le .h de l'écran lcd
		write_lcd(&lcd_screen_dev, HELLO_MSG, LCD_LINE_1);
		

	// déclaration utiles pour l'ADC (capteur d'humidité analogique) et le DHT11 (capteur humidité et temp , numérique):

		
		uint16_t buf;
		int temperature_ = 0;
		int humidite_ = 0;
		int err;

		struct adc_sequence sequence = {
			.buffer = &buf,
			/* buffer size in bytes, not number of samples */
			.buffer_size = sizeof(buf),
		};

	/* configuration des channels pour l'ADC*/
		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++)
		{
			if (!adc_is_ready_dt(&adc_channels[i]))
			{
				printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
				return 0;
			}

			err = adc_channel_setup_dt(&adc_channels[i]);
			if (err < 0)
			{
				printk("Could not setup channel #%d (%d)\n", i, err);
				return 0;
			}
		}

	while (1)
	{
		// interruption en cas de bouton gauche pressé, avec affichage d'un message sur l'écran LCD:
		int a = gpio_pin_get_dt(&bouton_G);

		if ( a >= 1 ){
			write_lcd(&lcd_screen_dev, BOUTON_PRESSE_MESSAGE, LCD_LINE_1);

		}

		// HUMIDITE PAR L'ADC ###########

		uint32_t count = 0;

		printk("ADC reading[%u]:\n");
		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++)
		{
			int32_t val_mv;

			printk("- %s, channel %d: ",
				   adc_channels[i].dev->name,
				   adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read_dt(&adc_channels[i], &sequence);
			if (err < 0)
			{
				printk("Could not read (%d)\n", err);
				continue;
			}

			if (adc_channels[i].channel_cfg.differential)
			{
				val_mv = (int32_t)((int16_t)buf);
			}
			else
			{
				val_mv = (int32_t)buf;
			}
			printk("%" PRId32, val_mv);

			/* conversion to mV may not be supported, skip if not */

			printk(" = %" PRId32 " mV\n", val_mv);
		}

		// TEMP ET HUMIDITE PAR LE DHT11 ###########

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

		k_sleep(K_SECONDS(10)); // PAUSE du système durant 10 secondes pour que le relevé et affichage se fasse toutes les 10sec

	} // fin du while

} // fin du main
