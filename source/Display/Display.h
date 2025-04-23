#pragma once

#include "hardware/i2c.h"
#include <string.h>
#include <math.h>
#include "Font/font.h"
#include "Vec2.h"
#include "Buffer.h"
#include "Elements.h"


#define SDD1315

namespace Display{

    #ifdef SDD1315
    #define OLED_ADDR 0x3C
    
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