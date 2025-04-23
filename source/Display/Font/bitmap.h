#pragma once
#include "font.h"

struct bitmap{
    uint8_t width;
    uint8_t height;
    uint8_t* bitmap;

    ~bitmap(void){
        delete[] bitmap;
    }
};