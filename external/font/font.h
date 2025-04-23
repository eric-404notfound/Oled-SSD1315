


#include "ascii.h"
struct bitmap{
    uint8_t width;
    uint8_t height;
    uint8_t* bitmap;

    ~bitmap(void){
        delete[] bitmap;
    }
};

#include "latin1.h"
#include "special_symbols.h"



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
