#include <iostream>
#include <cmath>
#include "player.h"
 
Menu::Menu() : wants_to_quit(false), current(Main), mouse_down(false), difficulty(Normal),
    buttons({
        Button(550, 400, 140, 55), //play
        Button(380, 470, 600, 55), //difficulty
        Button(550, 540, 140, 55), //menu quit
        Button(500, 300, 220, 55), //resume
        Button(530, 370, 140, 55) //pause quit
    })
{

}
/*
Menu& Menu::operator=(Menu p) 
{
    map = p.map;
    return *this;
}
*/
bool Menu::check_hover(ushort button_id)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    bool hover = buttons[button_id].is_mouse_over(ushort(x), ushort(y));
    if(hover && mouse_down)
        handle_click(button_id);
    return hover;
}

void Menu::handle_click(ushort button_id)
{
    if(button_id == 0) //play
        current = None;
    else if(button_id == 1) //difficulty
        difficulty = (Difficulty)(difficulty != 2 ? difficulty + 1 : 0);
    else if(button_id == 2 || button_id == 4) //quit
        wants_to_quit = true;
    else if(button_id == 3) //resume
        current = None;
}

Menu::~Menu()
{
    std::cout<<"Menu deleted"<<std::endl;
}