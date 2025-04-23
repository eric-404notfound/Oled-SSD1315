#include "Display.h"


namespace Display
{

    Buffer::Buffer(size_t _with, size_t _hight): pwidth(_with), pheight(_hight) {
                buffer = new uint8_t[pwidth * pheight/8];
                memset(buffer, 0, pwidth * pheight/8);
            }
        
    Buffer::~Buffer(void){delete[] buffer;}
        
    uint8_t* Buffer::get_buffer(void){return buffer;}
    size_t Buffer::get_with(void){return pwidth;}
    size_t Buffer::get_height(void){return pheight;}
        
    bool Buffer::get_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return false;
        return buffer[x + (y/8) * pwidth] & (1 << (y % 8));
    }
    void Buffer::set_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return;
        buffer[x + (y/8) * pwidth] |= (1 << (y % 8));
    }
    void Buffer::clear_pixel(size_t x, size_t y){
        if (x >= pwidth || y >= pheight) return;
        buffer[x + (y/8) * pwidth] &= ~(1 << (y % 8));
    }

    void Buffer::clear(void){
        memset(buffer, 0, pwidth * pheight/8);
    }

    void Buffer::set_pixels(size_t x, size_t y, size_t width, size_t height){
        if (x >= pwidth || y >= pheight) return;
        
        for (size_t i = 0; i < width; i++) {
            if (x + i >= pwidth) break;
        
            size_t j = 0;
            while (j < height) {
                size_t abs_y = y + j;
                uint8_t bit_start = abs_y % 8;
                uint8_t bits_available = 8 - bit_start;
                uint8_t bits_to_invert = (height - j < bits_available) ? (height - j) : bits_available;
        
                uint8_t mask = ((1 << bits_to_invert) - 1) << bit_start;
        
                buffer[(x + i) + (abs_y / 8) * pwidth] |= mask;
        
                j += bits_to_invert;
            }
        }
    }
    void Buffer::clear_pixels(size_t x, size_t y, size_t width, size_t height){
        if (x >= pwidth || y >= pheight) return;
        
        for (size_t i = 0; i < width; i++) {
            if (x + i >= pwidth) break;
        
            size_t j = 0;
            while (j < height) {
                size_t abs_y = y + j;
                uint8_t bit_start = abs_y % 8;
                uint8_t bits_available = 8 - bit_start;
                uint8_t bits_to_invert = (height - j < bits_available) ? (height - j) : bits_available;
        
                uint8_t mask = ((1 << bits_to_invert) - 1) << bit_start;
        
                buffer[(x + i) + (abs_y / 8) * pwidth] &= ~mask;
        
                j += bits_to_invert;
            }
        }
    }
    void Buffer::invert_pixels(size_t x, size_t y, size_t width, size_t height){
        if (x >= pwidth || y >= pheight) return;
        
        for (size_t i = 0; i < width; i++) {
            if (x + i >= pwidth) break;
        
            size_t j = 0;
            while (j < height) {
                size_t abs_y = y + j;
                uint8_t bit_start = abs_y % 8;
                uint8_t bits_available = 8 - bit_start;
                uint8_t bits_to_invert = (height - j < bits_available) ? (height - j) : bits_available;
        
                uint8_t mask = ((1 << bits_to_invert) - 1) << bit_start;
        
                buffer[(x + i) + (abs_y / 8) * pwidth] ^= mask;
        
                j += bits_to_invert;
            }
        }
    }

    void Buffer::place_bitmap(size_t x, size_t y, const bitmap* bmap){
        if (x >= pwidth || y >= pheight) return;
        for (size_t i = 0; i < bmap->width; i++){
            if (x + i >= pwidth) break;
            for (size_t j = 0; j < bmap->height; j++){
                if (y + j >= pheight) break;
                if ((1 << (j % 8)) & *(bmap->bitmap + i + (j/8) * bmap->width)){
                    set_pixel(x + i, y + j);
                } else {
                    clear_pixel(x + i, y + j);
                }
            }
        }
    }

    #ifdef SDD1315

        
    void Oled_SSD1315::init(void){
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

    void Oled_SSD1315::cmd(uint8_t command){
        uint8_t data[2] = {0x00, command};
        i2c_write_blocking(i2c1, OLED_ADDR, data, 2, false);
    }

    void Oled_SSD1315::update(void) {
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

    void Oled_SSD1315::clear(void) {
        memset(display.get_buffer(), 0, hight * width);
        update();
    }

    Text Oled_SSD1315::create_text(size_t x, size_t y) {
        return Text(&display, x , y); // Zugriff zum Lesen & Schreiben
    }
    Line Oled_SSD1315::create_line(size_t x1, size_t y1, size_t x2, size_t y2) {
        Vec2 p1 = {x1, y1};
        Vec2 p2 = {x2, y2};
        return Line(&display, p1, p2);
    }
    Box Oled_SSD1315::create_box(size_t x1, size_t y1, size_t x2, size_t y2) {
        Vec2 p1 = {x1, y1};
        Vec2 p2 = {x2, y2};
        return Box(&display, p1, p2);
    }
    Circle Oled_SSD1315::create_circle(size_t x, size_t y, size_t r) {
        Vec2 p1 = {x, y};
        return Circle(&display, p1, r);
    }

    Oled_SSD1315::Oled_SSD1315(size_t hight, size_t width): hight(hight/8), width(width), display(width, hight) {
        init();
    }

    Oled_SSD1315::~Oled_SSD1315(void){};
    #endif
};
