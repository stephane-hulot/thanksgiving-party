#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "player.h"

Player::Player() : wants_to_quit(false), x(3), y(3), angle(0), turn(0), walk_x(0), walk_y(0),
    speed(30), turn_speed(35), pressed_keys(NULL)
{
    pressed_keys = new bool[7];
}

void Player::handle_events(float dt)
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        wants_to_quit = SDL_QUIT == event.type || (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym);

        if(event.type == SDL_KEYUP)
            update_key(event.key.keysym.sym, false);

        if(event.type == SDL_KEYDOWN)
        {
            update_key(event.key.keysym.sym, true);

            if(event.key.keysym.sym == SDLK_SPACE)
                Fire();
        }
    }

    //Forward - backward movement
    if(pressed_keys[0])
        walk_y = speed * dt;
    else if(pressed_keys[2])
        walk_y = -speed * dt;
    else
        walk_y = 0;

    //Left - right movement
    if(pressed_keys[3])
        walk_x = speed * dt * 0.5;
    else if(pressed_keys[1])
        walk_x = -speed * dt * 0.5;
    else
        walk_x = 0;

    //rotation
    if(pressed_keys[5])
        turn = turn_speed * dt;
    else if(pressed_keys[4])
        turn = -turn_speed * dt;
    else
        turn = 0;

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

    //if(map[int(nx) + int(ny) * mapw] != ' ')
    //    map[int(nx) + int(ny) * mapw] = ' ';
}

void Player::update_key(SDLKey key, bool state)
{
    if (key == 'z') pressed_keys[0] = state;
    else if (key == 'q') pressed_keys[1] = state;
    else if (key == 's') pressed_keys[2] = state;
    else if (key == 'd') pressed_keys[3] = state;
    else if (key == SDLK_LEFT) pressed_keys[4] = state;
    else if (key == SDLK_RIGHT) pressed_keys[5] = state;
    else if (key == SDLK_SPACE) pressed_keys[6] = state;
}

void Player::Fire()
{

}

float Player::get_x()
{
    return x;
}

float Player::get_y()
{
    return y; 
}

float Player::get_angle()
{
    return angle;
}
