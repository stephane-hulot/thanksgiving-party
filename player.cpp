#include <iostream>
#include <cmath>
#include "player.h"

Player::Player(Map* m) : wants_to_quit(false), display_flash(false), menu(true), pause_menu(false), game_over(false), score(0), health(100), menu_selection(1), difficulty_selection(1), x(3), y(3), angle(0), turn(0),
    walk_x(0), walk_y(0), speed(30), turn_accel(0.18), turn_max(0.08), pressed_keys(NULL), map(m)
{
    pressed_keys = new bool[7];
    for(int i = 0; i < 7; i++)
    {
        pressed_keys[i] = false;
    }
}

Player::Player(const Player& p) : wants_to_quit(false), display_flash(false), menu(true), pause_menu(false), game_over(false), score(0), health(100), menu_selection(1), difficulty_selection(1), x(3), y(3), angle(0), turn(0),
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
    game_over = health < 1;
    if (SDL_PollEvent(&event))
    {

        if(event.type == SDL_KEYUP)
        {
            update_key(event.key.keysym.sym, false);
            
        }

        if(event.type == SDL_KEYDOWN)
        {
            update_key(event.key.keysym.sym, true);

            if(event.key.keysym.sym == SDLK_SPACE)
            {
                if(menu == false && pause_menu == false && game_over == false) Fire();
            }
        }
    }

    if(menu)
    {
        if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == 's' && menu_selection != 3) menu_selection += 1;
            else if(event.key.keysym.sym == 'z' && menu_selection != 1) menu_selection -= 1;
            else if(event.key.keysym.sym == 'd' && difficulty_selection != 3 && menu_selection == 2) difficulty_selection += 1;
            else if(event.key.keysym.sym == 'q' && difficulty_selection != 1 && menu_selection == 2) difficulty_selection -= 1;
            else if(event.key.keysym.sym == SDLK_SPACE && menu_selection == 1) menu = false;
            else if(event.key.keysym.sym == SDLK_SPACE && menu_selection == 3) wants_to_quit = true;
        }

        angle += dt * 0.1;
        return;
    }

    if(event.type == SDL_KEYUP)
    {
        if(event.key.keysym.sym == SDLK_ESCAPE)
        {
            if(menu == false && pause_menu == false && game_over == false) pause_menu = true;
        }
    }

    if(pause_menu)
    {
        map->speed = 0;
        if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == 's' && menu_selection != 2) menu_selection += 1;
            else if(event.key.keysym.sym == 'z' && menu_selection != 1) menu_selection -= 1;
            else if(event.key.keysym.sym == SDLK_SPACE && menu_selection == 1) 
                {
                    pause_menu = false;
                    map->speed = 0.03;
                }
            else if(event.key.keysym.sym == SDLK_SPACE && menu_selection == 2) wants_to_quit = true;
        }
    }

    if(game_over)
    {
        if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_ESCAPE) wants_to_quit = true;
        }
    }

    //Forward - backward movement
    if(pressed_keys[0] && pause_menu == false)
        walk_y = speed * dt;
    else if(pressed_keys[2] && pause_menu == false)
        walk_y = -speed * dt;
    else
        walk_y = 0;

    //Left - right movement
    if(pressed_keys[3] && pause_menu == false)
        walk_x = speed * dt * 0.5;
    else if(pressed_keys[1] && pause_menu == false)
        walk_x = -speed * dt * 0.5;
    else
        walk_x = 0;

    //rotation
    if(fabs(turn) < turn_max && pause_menu == false)
    {
        if(pressed_keys[5])
            turn += turn_accel * dt;
        else if(pressed_keys[4])
            turn -= turn_accel * dt;
    }
    if((!pressed_keys[5] && turn > 0 && pause_menu == false) || (!pressed_keys[4] && turn < 0 && pause_menu == false))
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
                    score += 10;
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