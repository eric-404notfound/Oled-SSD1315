#pragma once

#include "hardware/i2c.h"
#include <string.h>
#include <math.h>
#include "./external/font/font.h"
#include "Display/Elements.h"

struct Vec2{
    size_t x;
    size_t y;
};

#define SDD1315


namespace Display{

    class Buffer{
        private:
            uint8_t* buffer;
            size_t pwidth;
            size_t pheight;
        
        public:
            Buffer(size_t _with, size_t _hight);
        
            ~Buffer(void);
        
            uint8_t* get_buffer(void){return buffer;}
            size_t get_with(void){return pwidth;}
            size_t get_height(void){return pheight;}

            bool get_pixel(size_t x, size_t y);
            void set_pixel(size_t x, size_t y);
            void clear_pixel(size_t x, size_t y);
            void clear(void);
            void set_pixels(size_t x, size_t y, size_t width, size_t height);
            void clear_pixels(size_t x, size_t y, size_t width, size_t height);
            void invert_pixels(size_t x, size_t y, size_t width, size_t height);
        
            void place_bitmap(size_t x, size_t y, const bitmap* bmap);
        };

    #ifdef SDD1315
    class Oled_SSD1315 {
        private:
            
            size_t hight;
            size_t width;
            
            Buffer display;
            
        private:
            
            void init(void);
            void cmd(uint8_t command);

        public:
            
            
            void update(void);
            void clear(void);

            Text create_text(size_t x, size_t y);
            Line create_line(size_t x1, size_t y1, size_t x2, size_t y2);
            Box create_box(size_t x1, size_t y1, size_t x2, size_t y2);
            Circle create_circle(size_t x, size_t y, size_t r);
            
            Oled_SSD1315(size_t hight, size_t width);
            
            ~Oled_SSD1315(void);
            
            
    };
    #endif
        


}