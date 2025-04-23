#pragma once
#include "Display.h"

namespace Display{

    class Buffer{
        private:
            uint8_t* buffer;
            size_t pwidth;
            size_t pheight;
        
        public:
            Buffer(size_t _with, size_t _hight);
        
            ~Buffer(void);
        
            uint8_t* get_buffer(void);
            size_t get_with(void);
            size_t get_height(void);

            bool get_pixel(size_t x, size_t y);
            void set_pixel(size_t x, size_t y);
            void clear_pixel(size_t x, size_t y);
            void clear(void);
            void set_pixels(size_t x, size_t y, size_t width, size_t height);
            void clear_pixels(size_t x, size_t y, size_t width, size_t height);
            void invert_pixels(size_t x, size_t y, size_t width, size_t height);
        
            void place_bitmap(size_t x, size_t y, const bitmap* bmap);
        };
}