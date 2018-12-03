#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <SDL2/SDL.h>
#include "map.h"
#include "button.h"
#include "menu.h"

class Player
{
	public:
        Player(Map* ma, Menu* me);
        void handle_events(float dt);
        bool display_flash = false;
        int score = 0;
        int health = 100;
        ushort key_count = 0;

        float get_x();
        float get_y();
        float get_angle();

        ~Player();

        //copy constructor to avoid warning in c++11
        Player(const Player& p) : Player(p.map, p.menu) {};
        Player& operator=(const Player& p);

    private:
        float x, y, angle; //player position and rotation
        float turn, walk_x, walk_y; // player input
        float speed, turn_accel, turn_max;
        bool* pressed_keys;

        void update_key(SDL_Keycode key, bool state);
        void Fire();

        Map* map;
        Menu* menu;
};

#endif