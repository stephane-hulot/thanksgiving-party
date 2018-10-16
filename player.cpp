#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "player.h"

Player::Player() : wants_to_quit(false), x(3), y(3), angle(0), turn(0), walk_x(0), walk_y(0)
{

}

void Player::handle_events()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        wants_to_quit = SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym);

        if(event.type == SDL_KEYUP)
        {
            auto key = event.key.keysym.sym;
            if ('q' == key || 'd' == key) walk_x = 0;
            if ('z' == key || 's' == key) walk_y = 0;
            if (SDLK_RIGHT == key || SDLK_LEFT == key) turn = 0;
        }
        if(event.type == SDL_KEYDOWN)
        {
            auto key = event.key.keysym.sym;
            if ('q' == key) walk_x = -0.5;
            if ('d' == key) walk_x = 0.5;
            if ('z' == key) walk_y = 1;
            if ('s' == key) walk_y = -1;
            if (SDLK_RIGHT == key) turn = 1;
            if (SDLK_LEFT == key) turn = -1;
        }
    }

    angle += float(turn) * 0.05;
    float nx = x + (walk_y * cos(angle) + walk_x * cos(angle + M_PI / 2.0)) * 0.1;
    float ny = y + (walk_y * sin(angle) + walk_x * sin(angle + M_PI / 2.0)) * 0.1;
    if(angle > M_PI)
        angle -= 2 * M_PI;
    if(angle < -M_PI)
        angle += 2 * M_PI;

    if (int(nx) >= 0 && int(nx) < mapw && int(ny) >= 0 && int(ny) < maph && map[int(nx) + int(ny) * mapw] == ' ')
    {
        x = nx;
        y = ny;
    }
}

float Player::get_pos_x()
{
    return x;
}

float Player::get_pos_y()
{
    return y; 
}

float Player::get_angle()
{
    return angle;
}
