
struct bitmap{
    uint8_t width;
    uint8_t height;
    uint8_t* bitmap;

    ~bitmap(void){
        delete[] bitmap;
    }
};


#include "ascii.h"
#include "latin1.h"
#include "special_symbols.h"
