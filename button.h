#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <SDL2/SDL.h>

class Button
{
	public:
        Button(ushort x, ushort y, ushort w, ushort h);
        bool is_mouse_over(ushort mouse_x, ushort mouse_y); // returns true if the specified coordinates are inside the button

        //forbids copy constructor to avoid warning in c++11
        Button(const Button& b) : Button(b.x, b.y, b.w, b.h) {};
        Button& operator=(const Button& b) = delete;
    private:
        ushort x, y, w, h;
};

#endif