#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "player.h"

Player::Player() : wants_to_quit(false), x_(3), y_(3), angle_(0), turn_(0), walk_(0) {}

void Player::handle_events()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {	
        wants_to_quit = SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym);
        if (SDL_KEYUP == event.type)
        {
            if ('q'==event.key.keysym.sym || 'd'==event.key.keysym.sym) turn_ = 0;
            if ('z'==event.key.keysym.sym || 's'==event.key.keysym.sym) walk_ = 0;
        }
        if (SDL_KEYDOWN == event.type)
        {
            if ('q'==event.key.keysym.sym) turn_ = -1;
            if ('d'==event.key.keysym.sym) turn_ =  1;
            if ('z'==event.key.keysym.sym) walk_ =  1;
            if ('s'==event.key.keysym.sym) walk_ = -1;
        }
    }

    angle_ += float(turn_)*.05;
    float nx = x_ + walk_*cos(angle_)*.1;
    float ny = y_ + walk_*sin(angle_)*.1;

    if (int(nx) >= 0 && int(nx) < mapw && int(ny) >= 0 && int(ny) < maph && map[int(nx) + int(ny) * mapw] == ' ')
    {
        x_ = nx;
        y_ = ny;
    }
}

float Player::get_pos_x()
{
    return x_;
}

float Player::get_pos_y()
{
    return y_; 
}

float Player::get_angle()
{
    return angle_;
}
