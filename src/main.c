#include "lcd_screen_i2c.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>


#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_SCREEN_NODE DT_ALIAS(lcd_screen)

static const struct i2c_dt_spec lcd_screen_dev = I2C_DT_SPEC_GET(LCD_SCREEN_NODE);
const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});
const struct device *const dht11 = DEVICE_DT_GET_ONE(aosong_dht);

struct sensor_value temperature;
struct sensor_value humidity;



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
	
	k_sleep(K_SECONDS(10));

	}

	



}