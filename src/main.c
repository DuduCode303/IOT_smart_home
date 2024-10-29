#include "lcd_screen_i2c.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>


#define LED_YELLOW_NODE DT_ALIAS(led_yellow)
#define LCD_SCREEN_NODE DT_ALIAS(lcd_screen)

static const struct i2c_dt_spec lcd_screen_dev = I2C_DT_SPEC_GET(LCD_SCREEN_NODE);
const struct gpio_dt_spec led_yellow_gpio = GPIO_DT_SPEC_GET_OR(LED_YELLOW_NODE, gpios, {0});


//##############################################################################################
// MAIN #########################################################################################

int main() {

	// Init GPIO
	gpio_pin_configure_dt(&led_yellow_gpio, GPIO_OUTPUT_HIGH);

    // Init device
    init_lcd(&lcd_screen_dev);

    // Display a message


    write_lcd(&lcd_screen_dev, HELLO_MSG, LCD_LINE_1);
    //write_lcd_clear(&dev_lcd_screen, ZEPHYR_MSG LCD_LINE_2);
}