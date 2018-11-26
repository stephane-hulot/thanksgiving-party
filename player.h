#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <SDL2/SDL.h>
#include "map.h"

class Player
{
	public:
        Player(Map* m);
        void clean(); // could be moved to the destructor, however shared_ptr would be needed for the member pointers (c11)
        void handle_events(float dt);
        bool wants_to_quit, display_flash, menu, pause, game_over = false;
        int menu_select = 0;
        int difficulty = 0;
        int score = 0;
        int health = 100;
        ushort key_count = 0;

        

        float get_x();
        float get_y();
        float get_angle();

        ~Player();

        //copy constructor to avoid warning in c++11
        Player(const Player& p) : Player(p.map) {};
        Player& operator=(Player p);

    private:
        float x, y, angle; //player position and rotation
        float turn, walk_x, walk_y; // player input
        float speed, turn_accel, turn_max;
        bool* pressed_keys;

        void update_key(SDL_Keycode key, bool state);
        void Fire();

        Map* map;
};

#endif