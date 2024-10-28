#include <lcd_screen_i2c.h>
#include <lcd_screen_i2c.c>

//##############################################################################################
// MAIN #########################################################################################

int main() {

    // Init device
    init_lcd(&dev_lcd_screen);

    // Display a message
    write_lcd(&dev_lcd_screen, HELLO_MSG, LCD_LINE_1);
    write_lcd_clear(&dev_lcd_screen, ZEPHYR_MSG LCD_LINE_2);
}