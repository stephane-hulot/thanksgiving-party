#include <iostream>
#include <cmath>
#include "menu.h"
 
Menu::Menu() : wants_to_quit(false), current(Main), mouse_down(false), difficulty(Normal), sound(LowSound), timer(), leaderboard(),
    buttons({
        Button(550, 350, 140, 55), //play
        Button(380, 420, 600, 55), //difficulty
        Button(550, 620, 140, 55), //menu quit
        Button(500, 300, 220, 55), //resume
        Button(530, 370, 140, 55), //pause quit
        Button(550, 560, 140, 55), //menu help
        Button(550, 600, 140, 55), //help back
        Button(550, 600, 140, 55), //win quit
        Button(460, 490, 300, 55) //sound
    })
{

}

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
    {
        current = None;
        timer.start();
    }
    else if(button_id == 1) //difficulty
        difficulty = (Difficulty)(difficulty != 2 ? difficulty + 1 : 0);
    else if(button_id == 2 || button_id == 4 || button_id == 7) //quit
        wants_to_quit = true;
    else if(button_id == 3) //resume
        current = None;
    else if(button_id == 5) //menu help
        current = Help;
    else if(button_id == 6) //help back
        current = Main;
    else if(button_id == 8) //sound
        sound = (SoundVolume)(sound != 2 ? sound + 1 : 0);
}

Menu::~Menu()
{
    std::cout<<"Menu deleted"<<std::endl;
}