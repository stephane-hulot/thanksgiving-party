#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "game.h"

Game::Game() : x_(3.456), y_(3.456), angle_(0), turn_(0), walk_(0), 
    sdl_screen_(NULL), textures_(NULL), ntextures(0), texsize(0), game_running_(false) {}

bool Game::init_sdl(const char* title, int width, int height, int bpp)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cerr << "SDL_Init failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }
    SDL_WM_SetCaption(title, NULL);
    sdl_screen_ = SDL_SetVideoMode(width, height, 8*bpp, 0);
    if (!sdl_screen_)
    {
        std::cerr << "SDL_SetVideoMode failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }
    game_running_ = true;

    SDL_ShowCursor(SDL_DISABLE);

    textures_ = SDL_LoadBMP("walltext.bmp");
    if (!textures_)
    {
        std::cerr << "Couldn't load texture file " << SDL_GetError() << std::endl;
        return false;
    }
    if (textures_->format->BytesPerPixel!=bpp)
    {
        std::cerr << "Game screen bpp does not match textures bpp" << std::endl;
        return false;
    }
    texsize = textures_->h;
    ntextures = textures_->w / texsize;
    if (textures_->w != ntextures*texsize)
    {
        std::cerr << "Incorrect textures file: its width must be a multiple of its height" << std::endl;
    }
    std::cout << "Successfully read " << ntextures << " textures of " << texsize << "x" << texsize << " pixels" << std::endl;

    return true;
}

void Game::draw()
{
    SDL_FillRect(sdl_screen_, NULL, SDL_MapRGB(sdl_screen_->format, 255, 255, 255)); // TODO check for bpp

    int width = sdl_screen_->w; //screen width
    int height = sdl_screen_->h; //screen height

    for (int i = 0; i < width; i++) //for each vertical line of pixel
    {  
        //angle of the ray in rad
        float ray_angle = (1.0 - i / float(width)) * (angle_ - fov / 2.0) + i / float(width) * (angle_ + fov / 2.0);

        for (float dist = 0; dist < 20; dist += 0.05) //makes the ray move forward step by step
        {
            //pos of the tip of the ray for this iteration
            float ray_x = x_ + cos(ray_angle) * dist;
            float ray_y = y_ + sin(ray_angle) * dist;

            int tile_id = int(ray_x) + int(ray_y) * mapw; //id of current tile
            if (map[tile_id] != ' ') //the current tile is not empty, we hit a wall
            {
                // height of the current vertical segment to draw
                int wall_height = height / dist;
                
                ray_x -= floor(ray_x + 0.5);
                ray_y -= floor(ray_y + 0.5);

                // x-texcoord, we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
                int texture_x = (std::abs(ray_x) > std::abs(ray_y) ? ray_x : ray_y) * texsize;
                if(texture_x < 0)
                    texture_x += texsize;

                int wall_tex = map[tile_id]-'0';
                for (int texture_y = 0; texture_y < wall_height; texture_y++)
                {
                    // we need to scale texsize to wall_height, thus y-texcoord can be computed as texture_y * 64 / wall_height
                    putpixel(i, texture_y + (height - wall_height) / 2, getpixel(wall_tex, texture_x, (texture_y * 64) / wall_height));
                }

                //we hit a wall so we don't need to calculate the ray any further
                break;
            }
        }
    }

    SDL_Flip(sdl_screen_);
}

void Game::handle_events()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        game_running_ = !(SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym));
        if (SDL_KEYUP==event.type)
        {
            if ('q'==event.key.keysym.sym || 'd'==event.key.keysym.sym) turn_ = 0;
            if ('z'==event.key.keysym.sym || 's'==event.key.keysym.sym) walk_ = 0;
        }
        if (SDL_KEYDOWN==event.type)
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

    if (int(nx)>=0 && int(nx)<mapw && int(ny)>=0 && int(ny)<maph && map[int(nx)+int(ny)*mapw]==' ')
    {
        x_ = nx;
        y_ = ny;
    }
}

bool Game::running()
{
    return game_running_;
}

void Game::clean()
{
    if (sdl_screen_) SDL_FreeSurface(sdl_screen_);
    if (textures_  ) SDL_FreeSurface(textures_  );
    SDL_Quit();
}

// TODO these two functions work for 24 bits per pixel only
// TODO if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
Uint32 Game::getpixel(int itex, int x, int y)
{
    int texsize = textures_->h;
    if (itex<0 || itex>=ntextures || x<0 || y<0 || x>=texsize || y>=texsize) return 0;
    Uint8 *p = (Uint8 *)textures_->pixels + y*textures_->pitch + (x+texsize*itex)*textures_->format->BytesPerPixel;
    return p[0] | p[1] << 8 | p[2] << 16; 
}

void Game::putpixel(int x, int y, Uint32 pixel)
{
    if (x<0 || y<0 || x>=sdl_screen_->w || y>=sdl_screen_->h) return;
    int bpp = sdl_screen_->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)sdl_screen_->pixels + y*sdl_screen_->pitch + x*bpp;
    for (int i=0; i<bpp; i++)
    {
        p[i] = ((Uint8*)&pixel)[i];
    }
}

