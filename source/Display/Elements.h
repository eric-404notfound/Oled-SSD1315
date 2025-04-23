#pragma once
#include "Display.h"


namespace Display
{
    class Line{
        private:
            Buffer* display;
            Vec2 p1, p2;
        
        public:
            Line(Buffer* _display, Vec2 _p1, Vec2 _p2);
        
            ~Line();
        
            void draw(void);
            
        };

    class Text{

    private:
        Buffer* display;
        size_t offsetx;
        size_t offsety;
        char* text;

    public:
        Text(Buffer* _display, size_t _xoffset, size_t _yoffset);
        
        ~Text();

        Text& operator<<(const char* _text);

        void draw(void);

        void Highlight(void);

        char* get_text(void);



    };
    
    class Circle{
        private:
            Buffer* display;
            Vec2 center;
            size_t radius;
        public:
            Circle(Buffer* _display, Vec2 _center, size_t _radius);
        
            ~Circle();
        
            void draw(void);
        };

    class Box{
        private:
            Buffer* display;
            Line lines[4];
            Vec2 p1, p2;
        
            
        public:
        Box(Buffer* _display, Vec2 _p1, Vec2 _p2);
        ~Box();
            
        void fill();
        void clear();
        void clear_lines(void);
        void draw(void);
        void invert(void);
        
    };
}