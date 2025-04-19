#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include <string.h>

#include "font/font.h"

#define I2C_SDA_OLED 2
#define I2C_SCL_OLED 3

#define OLED_ADDR 0x3C

class Display_Text;
class Display_Segment;
class Oled_Display;


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
        if (bmap->width + x > pwidth || bmap->height + y > pheight) return;
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

class Display_Text{

private:
    Display_Buffer* display;
    size_t offsetx;
    size_t offsety;
    char* text;

public:
    Display_Text(Display_Buffer* _display, size_t _xoffset, size_t _yoffset): display(_display), offsetx(_xoffset), offsety(_yoffset){}
    
    ~Display_Text(){}

    Display_Text& operator<<(const char* text){
        this->text = (char*)text;
        for (size_t i = 0; i < strlen(text); i++){
            if (i * ascii_font_width + offsetx >= display->get_with()) {
                display->set_pixels(offsetx + (i-1) * special_symbols::PPP.width, offsety, &special_symbols::PPP);
                break;
            }
            bitmap bmap = get_ascii(text[i]);
            display->set_pixels(offsetx + i * bmap.width, offsety, &bmap);

        }
        return *this;
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

    Display_Text create_row(size_t y) {
        return Display_Text(&display, 0 , y); // Zugriff zum Lesen & Schreiben
    }

    Display_Text create_text(size_t x, size_t y) {
        return Display_Text(&display, x , y); // Zugriff zum Lesen & Schreiben
    }


    Oled_Display(size_t hight, size_t width): hight(hight/8), width(width), display(width, hight) {
        init();
    }

    ~Oled_Display(void){
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

    Display_Text Text1 = oled.create_row(0);
    Display_Text Text2 = oled.create_text(12, 48);

    Text1 << "Hallo du kleiner schlawiner";
    Text2 << "World!!!!!!!!!!!!!!";
    oled.update();

    uint i = 0;
    printf("\n");
    while (true){
        sleep_ms(1000);
        printf("\r%d", ++i);
    }
    return 0;
    
}