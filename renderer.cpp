#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "renderer.h"

Renderer::Renderer(Player* p) : sdl_screen_(NULL), textures_(NULL), ntextures(0), texsize(0), zbuffer(NULL), player(p)
{
    
}

bool Renderer::init_sdl(const char* title, ushort width, ushort height, unsigned char bpp)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cerr << "SDL_Init failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }

    SDL_WM_SetCaption(title, NULL);
    sdl_screen_ = SDL_SetVideoMode(width, height, 8*bpp, 0);

    if(!sdl_screen_)
    {
        std::cerr << "SDL_SetVideoMode failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }

    //SDL_ShowCursor(SDL_DISABLE);

    textures_ = SDL_LoadBMP("walltext.bmp");

    if(!textures_)
    {
        std::cerr << "Couldn't load texture file " << SDL_GetError() << std::endl;
        return false;
    }

    if(textures_->format->BytesPerPixel!=bpp)
    {
        std::cerr << "Game screen bpp does not match textures bpp" << std::endl;
        return false;
    }

    texsize = textures_->h;
    ntextures = textures_->w / texsize;

    if(textures_->w != ntextures*texsize)
    {
        std::cerr << "Incorrect textures file: its width must be a multiple of its height" << std::endl;
    }

    std::cout << "Successfully read " << ntextures << " textures of " << texsize << "x" << texsize << " pixels" << std::endl;

    zbuffer = new float[(int)sdl_screen_->w];

    return true;
}

void Renderer::draw()
{
    SDL_FillRect(sdl_screen_, NULL, SDL_MapRGB(sdl_screen_->format, 255, 255, 255)); // TODO check for bpp

    int width = sdl_screen_->w; //screen width
    int height = sdl_screen_->h; //screen height
    int middle = height / 2;

    for(int i = 0; i < width; i++) //for each vertical line of pixel
    {
        //angle of the ray in rad
        float ray_angle = (1.0 - i / float(width)) * (player->get_angle() - fov / 2.0) + i / float(width) * (player->get_angle() + fov / 2.0);

        float dist = 0;
        bool hit_wall = false;
        float projectDist = 0;

        //we don't need to calculate sin and cos for each step
        float cos_angle = cos(ray_angle);
        float sin_angle = sin(ray_angle);

        while(!hit_wall || dist > 40) //makes the ray move forward step by step
        {
            dist += 0.01;
            //pos of the tip of the ray for this iteration
            float ray_x = player->get_pos_x() + cos_angle * dist;
            float ray_y = player->get_pos_y() + sin_angle * dist;

            int tile_id = int(ray_x) + int(ray_y) * mapw; //id of current tile
            if(map[tile_id] != ' ') //the current tile is not empty, we hit a wall
            {
                hit_wall = true;
                // we need to project the distance onto a flat plane perpendicular to the player
                //in order to avoid fisheye effect
                projectDist = dist * cos(ray_angle - player->get_angle());
                // height of the current vertical segment to draw
                int wall_height = (height / projectDist);

                zbuffer[i] = dist;
                
                ray_x -= floor(ray_x + 0.5);
                ray_y -= floor(ray_y + 0.5);

                // x-texcoord, we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
                bool vertical = std::abs(ray_x) > std::abs(ray_y);
                int texture_x = (vertical ? ray_x : ray_y) * texsize;
                if(texture_x < 0)
                    texture_x += texsize;

                int wall_tex = map[tile_id]-'0';

                int wall_top = middle - wall_height / 2;
                int wall_bottom = wall_top + wall_height;

                for(int j = 0; j < height; j++)
                {
                    if(j < wall_top) //draw sky (top : 0,50,200 ; bottom : 150,200,200)
                        set_pixel(i, j, rgb_to_int(150 * (j / (float)height * 2), 50 + 150 * (j / (float)height * 2), 200));
                    else if(j >= wall_bottom) //draw ground (top : 0,100,0 ; bottom : 100,200,0)
                        set_pixel(i, j, rgb_to_int(100 - 100 * ((height - j) / (float)height * 2), 200 - 100 * ((height - j) / (float)height * 2), 0));
                    else //top < j < bottom, draw wall
                    {
                        int texture_y = (j - wall_top) / (float)wall_height * 64;
                        //displays the pixel and apply lighting
                        set_pixel(i, j, apply_light(get_pixel_tex(wall_tex, texture_x, texture_y), vertical ? 1 : 0.7));
                    }

                    //FPS weapon
                    int offset = width / 2 + 100;               
                    if(i < offset + 384 && i > offset && j > height - 384)
                    {
                        if(get_pixel_tex(4, (i - offset) / 6, (j - 6 - height) / 6 + 64) != rgb_to_int(0, 255, 255))
                            set_pixel(i, j, get_pixel_tex(4, (i - offset) / 6, (j - 6 - height) / 6 + 64));
                    }
                }
            }   
        } 
    }

    //TODO : add sorting based on the distance from the player
    draw_sprite(5, 4.5, 6.5, 600);
    draw_sprite(5, 3.5, 7.5, 600);
    draw_sprite(5, 12.5, 2, 600);
    draw_sprite(5, 7.5, 6.5, 600);

    SDL_Flip(sdl_screen_);
}

void Renderer::draw_sprite(ushort itex, ushort sprite_x, ushort sprite_y, ushort sprite_size)
{
    float sprite_dir = atan2(-player->get_pos_y()+ sprite_y, sprite_x - player->get_pos_x());
    float sprite_dist = sqrt((player->get_pos_x() - sprite_x) * (player->get_pos_x() - sprite_x) + (player->get_pos_y() - sprite_y) * (player->get_pos_y() - sprite_y));
    int sprite_width = std::min(800, (int)(1 / sprite_dist * sprite_size));
    int sprite_offset = (sprite_dir - player->get_angle()) * sdl_screen_->w + (sdl_screen_->w / 2) - (sprite_width / 2);
    int v_offset = 100;
    int middle = sdl_screen_->h / 2;
    int top = middle - sprite_width / 2 + v_offset / sprite_dist;
    int bottom = middle + sprite_width / 2 + v_offset / sprite_dist;
    float height = bottom - top;

    for(int x = sprite_offset; x < sprite_offset + sprite_width; x++)
    {   
        if(sprite_dist < zbuffer[x])
        {
            for(int y = top; y < bottom; y++)
            {
                int texture_x = (x - sprite_offset) / (float)(sprite_width) * 64;
                int texture_y = (y - top) / (float)height * 64;

                if(get_pixel_tex(itex, texture_x, texture_y) != rgb_to_int(0, 255, 255))
                    set_pixel(x, y, get_pixel_tex(itex, texture_x, texture_y));
            }
        }
    }
}

/*
void Renderer::display_framerate(ushort fps)
{
    TTF_Init();
    std::string text = std::to_string(fps) + " FPS";
    TTF_Font *font = TTF_OpenFont("FreeSans.ttf", 28);
    SDL_Color color = {0, 0, 0};
    SDL_Surface display = TTF_RenderText_Solid(font, text, color);
    SDL_Rect pos;
    pos.x = 10;
    pos.y = 10;
    SDL_BlitSurface(display, NULL, sdl_screen_, &pos);
}
*/

void Renderer::clean()
{
    if(sdl_screen_) SDL_FreeSurface(sdl_screen_);
    if(textures_  ) SDL_FreeSurface(textures_  );
    SDL_Quit();
}

// TODO these two functions work for 24 bits per pixel only
// TODO if (SDL_BYTEORDER == SDL_BIG_ENDIAN)

//Gets a pixel from the texture file
Uint32 Renderer::get_pixel_tex(ushort itex, ushort x, ushort y)
{
    int texsize = textures_->h;
    if(itex >= ntextures || x >= texsize || y >= texsize) 
        return 0;

    Uint8 *p = (Uint8 *)textures_->pixels + y * textures_->pitch + (x+texsize * itex) * textures_->format->BytesPerPixel;
    return p[0] | p[1] << 8 | p[2] << 16; 
}


//sets a pixel on the screen
void Renderer::set_pixel(ushort x, ushort y, Uint32 pixel)
{
    if (x >= sdl_screen_->w || y >= sdl_screen_->h)
        return;

    unsigned char bpp = sdl_screen_->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)sdl_screen_->pixels + y * sdl_screen_->pitch + x * bpp;
    for(unsigned char i = 0; i < bpp; i++)
    {
        p[i] = ((Uint8*)&pixel)[i];
    }
}

Uint32 Renderer::rgb_to_int(unsigned char r, unsigned char g, unsigned char b)
{
    return (r << 16) + (g << 8) + b;
}

Uint32 Renderer::apply_light(Uint32 color, float factor)
{
    Uint32 r = (color & 0x00FF0000) * factor;
    Uint32 g = (color & 0x0000FF00) * factor;
    Uint32 b = (color & 0x000000FF) * factor;

   return (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}