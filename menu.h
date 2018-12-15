#ifndef _MENU_H_
#define _MENU_H_

#include <SDL2/SDL.h>
#include "map.h"
#include "button.h"
#include "timer.h"
#include "leaderboard.h"

enum CurrentMenu {None, Main, Pause, GameOver, Win, Help};
enum Difficulty {Easy, Normal, Hard};
enum SoundVolume {NoSound, LowSound, NormalSound};

class Menu
{
	public:
        Menu();
        bool check_hover(ushort button_id); //returns true if the mouse is over a button and calls handme_click if a mouse button is pressed
        void handle_click(ushort button_id); //handle the buttons logic
        bool wants_to_quit = false; //true if the player want close the game
        CurrentMenu current = Main; //currently displayed menu
        bool mouse_down = false; //true if a mouse button down during this frame, this is handled by the class Player
        Difficulty difficulty = Normal;
        SoundVolume sound = LowSound;
        Timer timer;
        Leaderboard leaderboard;
        Button buttons[9]; //UI buttons

        ~Menu();

    private:
        
};

#endif