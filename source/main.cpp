#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include <string.h>

#include "font/font.h"

#define I2C_SDA_OLED 2
#define I2C_SCL_OLED 3

#define OLED_ADDR 0x3C

struct font8x8{
    uint8_t data[8];
};

class Display_Segment{
    private:
        uint8_t* buffer;

    public:
        Display_Segment(uint8_t* display): buffer(display){}

        void setPixel(uint8_t x, uint8_t y, bool value){
            if (value)
                buffer[x] |= (1 << y);
            else
                buffer[x] &= ~(1 << y);
        }
        Display_Segment& operator=(const char symbol){
            memcpy(buffer, &ascii_bmap[(symbol - 32)*ascii_font_width], 8);
            return *this;
        }
};

class Display_Row{

private:
    uint8_t* buffer;
    size_t len;
    size_t index;

public:
    Display_Row(uint8_t* display, size_t width): buffer(display), len(width), index(0){}
    
    ~Display_Row(void){}

    Display_Segment operator[](size_t index){
        if (index >= len)
            return Display_Segment(nullptr);
        return Display_Segment(buffer + index * ascii_font_width);
    }

    Display_Row& operator<<(const char* text){
        for (size_t i = 0; i < len; i++) {
            if (text[i] == '\0') break;
            (*this)[i] = text[i];

            }
        return *this;
    }



};

class Oled_Display {
private:

    size_t hight;
    size_t width;

    uint8_t* oled_buffer;

private:

public:


public:
    void cmd(uint8_t command){
        uint8_t data[2] = {0x00, command};
        i2c_write_blocking(i2c1, OLED_ADDR, data, 2, false);
    }

    void update(void) {
        // Setze Adressbereich (für horizontales Schreiben)
        cmd(0x21); cmd(0); cmd(127); // Spalten: 0–127
        cmd(0x22); cmd(0); cmd(7);   // Seiten: 0–7
    
        for (uint16_t i = 0; i < 1024; i += 16) {
            uint8_t data[17];
            data[0] = 0x40; // Control-Byte: RAM-Daten
            memcpy(&data[1], &oled_buffer[i], 16);
            i2c_write_blocking(i2c1, OLED_ADDR, data, 17, false);
        }
    }

    void clear(void) {
        memset(oled_buffer, 0, hight * width);
        update();
    }

    Display_Row operator[](size_t index) {
        return Display_Row(oled_buffer + index * width, width/ascii_font_width); // Zugriff zum Lesen & Schreiben
    }

    Oled_Display(size_t hight, size_t width): hight(hight/8), width(width){
    oled_buffer = new uint8_t[this->hight * width];
    memset(oled_buffer, 0, this->hight * width);

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

    ~Oled_Display(void){
        delete[] oled_buffer;
    };


};

static uint8_t oled_buffer[1024];

int main(){

    stdio_init_all();

    printf("\n\n\n\nHallo welt\n\n\n\n");

    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(I2C_SDA_OLED, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_OLED);
    gpio_pull_up(I2C_SCL_OLED);

    Oled_Display oled = Oled_Display(64, 128);

    oled[0] << "abcdefghijklmnopqrstuvwxyz";
    oled[1] << "ABCDEFGHIJKLMNOP";
    oled[2] << "0123456789";
    oled[3] << "!@#$%^&*()";
    oled[4] << "Hallo Welt";
    oled.update();

    uint i = 0;
    printf("\n");
    while (true){
        sleep_ms(1000);
        printf("\r%d", ++i);
    }
    return 0;
    
}