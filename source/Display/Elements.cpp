#include "Display.h"

namespace Display{


    Line::Line(Buffer* _display, Vec2 _p1, Vec2 _p2):display(_display), p1(_p1), p2(_p2) {this->draw();}
    
    Line::~Line(){}
    
    void Line::draw(void){
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
    
    
    Box::Box(Buffer* _display, Vec2 _p1, Vec2 _p2): lines{Line(_display, _p1, {_p2.x, _p1.y}) ,
                                                            Line(_display, {_p2.x, _p1.y}, _p2),
                                                            Line(_display, _p2, {_p1.x, _p2.y}),
                                                            Line(_display, {_p1.x, _p2.y}, _p1)},
                                                            display(_display),
                                                            p1(_p1), p2(_p2){}
    Box::~Box(){}

    void Box::draw(void){
        for (size_t i = 0; i < 4; i++){
            lines[i].draw();
        }
    }
    
    void Box::fill(){
        display->set_pixels(p1.x + 1, p1.y + 1, p2.x - p1.x - 1, p2.y - p1.y - 1);
    }

    void Box::clear(){
        display->clear_pixels(p1.x + 1, p1.y + 1, p2.x - p1.x - 1, p2.y - p1.y - 1);
    }
   
    void Box::clear_lines(void){
        // Todo
        }
    
    void Box::invert(void){
        display->invert_pixels(p1.x + 1, p1.y + 1, p2.x - p1.x - 1, p2.y - p1.y - 1);
    }
    
    
    Circle::Circle(Buffer* _display, Vec2 _center, size_t _radius): display(_display), center(_center), radius(_radius) {
        this->draw();
    }

    Circle::~Circle(){}

    void Circle::draw(void){
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




    Text::Text(Buffer* _display, size_t _xoffset, size_t _yoffset): display(_display), offsetx(_xoffset), offsety(_yoffset){}
    
    Text::~Text(){}

    Text& Text::operator<<(const char* _text){
        this->text = (char*)_text;
        this->draw();
        return *this;
    }

    void Text::draw(void){
        for (size_t i = 0; i < strlen(text); i++){
            bitmap bmap = get_ascii(text[i]);
            display->place_bitmap(offsetx + i * bmap.width, offsety, &bmap);
        }
    }

    void Text::Highlight(void){

        size_t text_length = strlen(text);
        display->invert_pixels(offsetx, offsety, text_length * ascii_font_width, ascii_font_hight);
    }

    char* Text::get_text(void){
        return text;
    }
    
    
}

