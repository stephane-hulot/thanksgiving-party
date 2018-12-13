#include <iostream>
#include "button.h"

Button::Button(ushort x, ushort y, ushort w, ushort h) : x(x), y(y), w(w), h(h)
{

}

bool Button::is_mouse_over(ushort mouse_x, ushort mouse_y)
{
	bool is_over = mouse_x < x || mouse_y < y || mouse_x > x + w || mouse_y > y + h ? false : true;
    return is_over;
}