#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cmath>
#include <SDL/SDL.h>
#include "map.h"

class Player
{
	public:
        Player();
        void clean(); // could be moved to the destructor, however shared_ptr would be needed for the member pointers (c11)
        void handle_events();
        bool wants_to_quit;

        float get_pos_x();
        float get_pos_y();
        float get_angle();

    private:
        float x, y, angle; //player position and rotation
        float turn, walk_x, walk_y; // player input

};

#endif