#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"

#include <string.h>
#include <math.h>


#include "Display/Display.h"

#define I2C_SDA_OLED 2
#define I2C_SCL_OLED 3




Display::Oled_SSD1315* static_oled;
Display::Text* static_time;

static void alarm_callback(void) {
    datetime_t t = {0};
    rtc_get_datetime(&t);
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", t.hour, t.min);
    *static_time << buffer;
    static_oled->update();
}

int main(){

    stdio_init_all();
    rtc_init();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    printf("\n\n\n\nHallo welt\n\n\n\n");

    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(I2C_SDA_OLED, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_OLED);
    gpio_pull_up(I2C_SCL_OLED);
    Display::Oled_SSD1315 oled(64, 128);
    static_oled = &oled;

    
    Display::Box Header = oled.create_box(0, 0, 127, 13);
    Display::Text Text1 = oled.create_text(1, 1);

    Text1 << "Pico OLED";
    Display::Box Zeit_Fenster = oled.create_box(86, 0, 127, 13);
    Display::Text zeit = oled.create_text(87, 1);
    Display::Box Würfel = oled.create_box(20, 20, 60, 60);
    Display::Box Würfel2 = oled.create_box(18,18, 62, 62);
    Display::Circle circle1 = oled.create_circle(30, 30, 5);
    Display::Circle circle2 = oled.create_circle(50, 30, 5);
    Display::Circle circle3 = oled.create_circle(50, 50, 5);
    Display::Circle circle4 = oled.create_circle(30, 50, 5);




    static_time = &zeit;
    oled.update();



    sleep_ms(1000);
    datetime_t t = {
        .year  = 2025,
        .month = 04,
        .day   = 21,
        .dotw  = 1, 
        .hour  = 22,
        .min   = 49,
        .sec   = 00
    };
    
    if (rtc_set_datetime(&t)) zeit << "!!:!!";
    else zeit << "??:??";

    
    oled.update();

    datetime_t alarm_t = {
        .year  = -1,
        .month = -1,
        .day   = -1,
        .dotw  = -1,
        .hour  = -1,
        .min   = -1,
        .sec   = 00
    }; 
    rtc_set_alarm(&alarm_t, alarm_callback);
    
    
    
    uint i = 0;
    while(true){
        sleep_ms(1000);
        Würfel.invert();
        oled.update();
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(1000);
        Text1.Highlight();
        oled.update();
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    }
    return 0;
    
}