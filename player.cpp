#include <iostream>
#include <cmath>
#include "player.h"

Player::Player(Map* m) : wants_to_quit(false), display_flash(false), x(3), y(3), angle(0), turn(0),
    walk_x(0), walk_y(0), speed(30), turn_accel(0.18), turn_max(0.08), pressed_keys(NULL), map(m)
{
    pressed_keys = new bool[7];
    for(int i = 0; i < 7; i++)
    {
        pressed_keys[i] = false;
    }
}

Player::Player(const Player& p) : wants_to_quit(false), display_flash(false), x(3), y(3), angle(0), turn(0),
    walk_x(0), walk_y(0), speed(30), turn_accel(0.18), turn_max(0.08), pressed_keys(NULL), map(NULL)
{
    map = p.map;
}

Player& Player::operator=(Player p)
{
    map = p.map;
    return *this;
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
    if(fabs(turn) < turn_max)
    {
        if(pressed_keys[5])
            turn += turn_accel * dt;
        else if(pressed_keys[4])
            turn -= turn_accel * dt;
    }
    if((!pressed_keys[5] && turn > 0) || (!pressed_keys[4] && turn < 0))
        turn = 0;

    angle += turn;
    if(angle > M_PI)
        angle -= 2 * M_PI;
    if(angle < -M_PI)   
        angle += 2 * M_PI;

    float nx = x + (walk_y * cos(angle) + walk_x * cos(angle + M_PI / 2.0)) * 0.1;
    float ny = y + (walk_y * sin(angle) + walk_x * sin(angle + M_PI / 2.0)) * 0.1;

    if(int(nx) >= 0 && int(nx) < map->w && int(ny) >= 0 && int(ny) < map->h)
    {
        if(map->get_tile(ushort(nx), ushort(y)) == ' ')
            x = nx;
        if(map->get_tile(ushort(x), ushort(ny)) == ' ')
            y = ny;
    }
}

void Player::update_key(SDL_Keycode key, bool state)
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
    std::vector<Sprite> sprites = map->get_sprites();

    float dist = 0;
    bool hit_wall = false;

    //we don't need to calculate sin and cos for each step
    float x_offset = cos(angle);
    float y_offset = sin(angle);

    while(!hit_wall && dist < 10) //makes the ray move forward step by step
    {
        dist += 0.1;

        //pos of the tip of the ray for this iteration
        float ray_x = x + x_offset * dist;
        float ray_y = y + y_offset * dist;

        if(map->get_tile(ushort(ray_x), ushort(ray_y)) != ' ') //the current tile is not empty, we hit a wall
        {
            hit_wall = true;
            if(map->get_tile(ushort(ray_x), ushort(ray_y)) == '2')
                map->set_tile(short(ray_x), ushort(ray_y), ' ');
        }
        else
        {
            for(unsigned int i = 0; i < sprites.size(); i++)
            {
                if(sprites.at(i).type != 1) continue;
                float sqr_dist = pow(ray_x - sprites.at(i).x, 2) + pow(ray_y - sprites.at(i).y, 2);
                if(sqr_dist < 0.015)
                {
                    map->delete_sprite(i);
                    hit_wall = true;
                    break;
                }
            }
        }
    }

    display_flash = true;
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

Player::~Player()
{
    delete pressed_keys;
    std::cout<<"Player deleted"<<std::endl;
}