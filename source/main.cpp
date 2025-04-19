#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "hardware/gpio.h"

#include <string.h>
#include <math.h>

#include "font/font.h"

#define I2C_SDA_OLED 2
#define I2C_SCL_OLED 3

#define OLED_ADDR 0x3C

static volatile bool fired = false;

struct Vec2{
    size_t x;
    size_t y;
};


bitmap get_ascii(char symbol){
    uint8_t width = ascii_font_width;
    uint8_t height = ascii_font_hight;
    size_t bitmap_offset = symbol - 32;
    uint8_t* bmap = new uint8_t[width * (height/8 + (height % 8 ? 1 : 0))];
    //uint8_t bmap2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (size_t i = 0; i < (height/8 + (height % 8 ? 1 : 0)); i++)
        memcpy(bmap + i * width, ascii_bmap + (bitmap_offset * ascii_font_width) +  i * ascii_font_width * ascii_font_char_count , width);
        //memcpy(bmap, bmap2, 32);
    return {width, height, bmap};
}

class Display_Buffer{
private:
    uint8_t* buffer;
    size_t pwidth;
    size_t pheight;

public:
    Display_Buffer(size_t _with, size_t _hight): pwidth(_with), pheight(_hight) {
        buffer = new uint8_t[pwidth * pheight/8];
        memset(buffer, 0, pwidth * pheight/8);
    }

    ~Display_Buffer(void){
        delete[] buffer;
    }

    uint8_t* get_buffer(void){
        return buffer;
    }
    size_t get_with(void){
        return pwidth;
    }
    size_t get_hight(void){
        return pheight;
    }
    uint8_t& operator[](size_t index){
        return *(buffer + index * pwidth);
    }

    bool get_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return false;
        return buffer[x + (y/8) * pwidth] & (1 << (y % 8));
    }
    void set_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return;
        buffer[x + (y/8) * pwidth] |= (1 << (y % 8));
    }
    void clear_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return;
        buffer[x + (y/8) * pwidth] &= ~(1 << (y % 8));
    }
    void set_pixels(size_t x, size_t y, const bitmap* bmap){
        if (x >= pwidth || y >= pheight) return;
        for (size_t i = 0; i < bmap->width; i++){
            for (size_t j = 0; j < bmap->height; j++){
                if ((1 << (j % 8)) & *(bmap->bitmap + i + (j/8) * bmap->width)){
                    set_pixel(x + i, y + j);
                } else {
                    clear_pixel(x + i, y + j);
                }
            }
        }
    }
};

class Display_Line{
private:
    Display_Buffer* display;
    Vec2 p1, p2;

public:
    Display_Line(Display_Buffer* _display, Vec2 _p1, Vec2 _p2):display(_display), p1(_p1), p2(_p2) {this->draw();}

    ~Display_Line(){}

    void draw(void){
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
        float x_inc = (float)dx / (float)steps;
        float y_inc = (float)dy / (float)steps;

        float x = p1.x;
        float y = p1.y;

        for (int i = 0; i <= steps; i++){
            display->set_pixel((int)x, (int)y);
            x += x_inc;
            y += y_inc;
        }
    }
};

class Display_Box{
    private:
        Display_Line lines[4];

    
    public:
    Display_Box(Display_Buffer* _display, Vec2 p1, Vec2 p2): lines{Display_Line(_display, p1, {p2.x, p1.y}) ,
                                                                    Display_Line(_display, {p2.x, p1.y}, p2),
                                                                    Display_Line(_display, p2, {p1.x, p2.y}),
                                                                    Display_Line(_display, {p1.x, p2.y}, p1)} {}
    
};

class Display_Circle{
private:
    Display_Buffer* display;
    Vec2 center;
    size_t radius;
public:
    Display_Circle(Display_Buffer* _display, Vec2 _center, size_t _radius): display(_display), center(_center), radius(_radius) {
        this->draw();
    }

    ~Display_Circle(){}

    void draw(void){
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        while (x <= y){
            display->set_pixel(center.x + x, center.y + y);
            display->set_pixel(center.x - x, center.y + y);
            display->set_pixel(center.x + x, center.y - y);
            display->set_pixel(center.x - x, center.y - y);
            display->set_pixel(center.x + y, center.y + x);
            display->set_pixel(center.x - y, center.y + x);
            display->set_pixel(center.x + y, center.y - x);
            display->set_pixel(center.x - y, center.y - x);

            if (d < 0){
                d += 4 * x + 6;
            } else {
                d += 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
};

class Display_Text{

private:
    Display_Buffer* display;
    size_t offsetx;
    size_t offsety;
    char* text;

public:
    Display_Text(Display_Buffer* _display, size_t _xoffset, size_t _yoffset): display(_display), offsetx(_xoffset), offsety(_yoffset){}
    
    ~Display_Text(){}

    Display_Text& operator<<(const char* _text){
        this->text = (char*)_text;
        this->draw();
        return *this;
    }

    void draw(void){
        for (size_t i = 0; i < strlen(text); i++){
            bitmap bmap = get_ascii(text[i]);
            display->set_pixels(offsetx + i * bmap.width, offsety, &bmap);
        }
    }

    char* get_text(void){
        return text;
    }



};

class Oled_Display {
private:

    size_t hight;
    size_t width;

    Display_Buffer display;

private:

    void init(void){
        // Setze Adressbereich (für horizontales Schreiben)
        cmd(0xAE);       // Display OFF
        cmd(0x20);       // Set Memory Addressing Mode
        cmd(0x10);       // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
        cmd(0xB0);       // Set Page Start Address for Page Addressing Mode,0-7
        cmd(0xC8);       // Set COM Output Scan Direction
        cmd(0x00);       // Set low column address
        cmd(0x10);       // Set high column address
        cmd(0x40);       // Set start line address
        cmd(0x81);       // Set contrast control register
        cmd(0xFF);      // Set segment re-map 0 to 127
        cmd(0xA1);       // Set normal display
        cmd(0xA6);       // 
        cmd(0xA8);       // Set multiplex ratio(1 to 64)
        cmd(0x3F);       // 
        cmd(0xA4);       // 
        cmd(0xD3);       // Set display offset
        cmd(0x00);       // 
        cmd(0xD5);
        cmd(0xF0);       // Set display clock divide ratio/oscillator frequency
        cmd(0xD9);       // Set pre-charge period
        cmd(0x22);       
        cmd(0xDA);       // Set COM pins hardware configuration
        cmd(0x12);
        cmd(0xDB);       // Set vcomh
        cmd(0x20);
        cmd(0x8D);       // Set DC-DC enable
        cmd(0x14);
        cmd(0xAF);       // Display ON
    }

    void cmd(uint8_t command){
        uint8_t data[2] = {0x00, command};
        i2c_write_blocking(i2c1, OLED_ADDR, data, 2, false);
    }
public:


    void update(void) {
        // Setze Adressbereich (für horizontales Schreiben)
        cmd(0x21); cmd(0); cmd(127); // Spalten: 0–127
        cmd(0x22); cmd(0); cmd(7);   // Seiten: 0–7
        // Daten schreiben
        uint8_t* dbuffer = display.get_buffer();
        for (uint16_t i = 0; i < 1024; i += 16) {
            uint8_t data[17];
            data[0] = 0x40; // Control-Byte: RAM-Daten
            memcpy(&data[1], &dbuffer[i], 16);
            i2c_write_blocking(i2c1, OLED_ADDR, data, 17, false);
        }
    }

    void clear(void) {
        memset(display.get_buffer(), 0, hight * width);
        update();
    }

    Display_Text create_text(size_t x, size_t y) {
        return Display_Text(&display, x , y); // Zugriff zum Lesen & Schreiben
    }
    Display_Line create_line(size_t x1, size_t y1, size_t x2, size_t y2) {
        Vec2 p1 = {x1, y1};
        Vec2 p2 = {x2, y2};
        return Display_Line(&display, p1, p2);
    }
    Display_Box create_box(size_t x1, size_t y1, size_t x2, size_t y2) {
        Vec2 p1 = {x1, y1};
        Vec2 p2 = {x2, y2};
        return Display_Box(&display, p1, p2);
    }
    Display_Circle create_circle(size_t x, size_t y, size_t r) {
        Vec2 p1 = {x, y};
        return Display_Circle(&display, p1, r);
    }

    Oled_Display(size_t hight, size_t width): hight(hight/8), width(width), display(width, hight) {
        init();
    }

    ~Oled_Display(void){
    };


};


Oled_Display* static_oled;
Display_Text* static_time;

static void alarm_callback(void) {
    datetime_t t = {0};
    rtc_get_datetime(&t);
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", t.hour, t.min);
    fired = true;
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
    Oled_Display oled(64, 128);
    static_oled = &oled;

    Display_Text Text1 = oled.create_text(1, 1);
    Display_Text Text2 = oled.create_text(5, 16);
    Display_Box Header = oled.create_box(0, 0, 127, 14);

    Text1 << "Pico OLED";
    Text2 << "Dieser_Text_ist@@@";
    Display_Box Zeit_Fenster = oled.create_box(86, 0, 127, 14);
    Display_Text zeit = oled.create_text(87, 1);
    Display_Circle circle2 = oled.create_circle(64, 25, 7);

    static_time = &zeit;
    oled.update();



    sleep_ms(1000);
    datetime_t t = {
        .year  = 2025,
        .month = 04,
        .day   = 20,
        .dotw  = 6, // 0 is Sunday, so 3 is Wednesday
        .hour  = 0,
        .min   = 19,
        .sec   = 00
    };
    
    rtc_set_datetime(&t);
    datetime_t alarm_t = {
        .year  = -1,
        .month = -1,
        .day   = -1,
        .dotw  = -1,
        .hour  = -1,
        .min   = -1,
        .sec   = 00
    }; 
    alarm_callback();
    rtc_set_alarm(&alarm_t, alarm_callback);
    
    

    uint i = 0;
    while(true){
        sleep_ms(1000);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(1000);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    }
    return 0;
    
}