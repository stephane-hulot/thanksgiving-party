#include <SDL/SDL.h>
#include <iostream>
#include <cmath>
#include "renderer.h"

Renderer::Renderer(Player* p, Map* m) : sdl_screen(NULL), wall_textures(NULL), sprites_textures(NULL), zbuffer(NULL), player(p), map(m)
{
    
}

bool Renderer::init_sdl(const char* title, ushort width, ushort height)
{
    //24 bits colors
    unsigned char bpp = 3;

    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        std::cerr << "SDL_Init failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }

    SDL_WM_SetCaption(title, NULL);
    sdl_screen = SDL_SetVideoMode(width, height, 8 * bpp, 0);

    if(!sdl_screen)
    {
        std::cerr << "SDL_SetVideoMode failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }

    //SDL_ShowCursor(SDL_DISABLE);

    wall_textures = SDL_LoadBMP("walltext.bmp");
    sprites_textures = SDL_LoadBMP("sprites.bmp");

    if(!wall_textures || !sprites_textures)
    {
        std::cerr << "Couldn't load texture file " << SDL_GetError() << std::endl;
        return false;
    }

    if(wall_textures->format->BytesPerPixel != bpp || sprites_textures->format->BytesPerPixel != bpp)
    {
        std::cerr << "Game screen bpp does not match textures bpp" << std::endl;
        return false;
    }
    if(wall_textures->w != (wall_textures->w / wall_textures->h) * wall_textures->h
        || sprites_textures->w != (sprites_textures->w / sprites_textures->h) * sprites_textures->h)
    {
        std::cerr << "Incorrect textures file: its width must be a multiple of its height" << std::endl;
        return false;
    }

    zbuffer = new float[(int)sdl_screen->w];

    return true;
}

void Renderer::draw()
{
    SDL_FillRect(sdl_screen, NULL, SDL_MapRGB(sdl_screen->format, 255, 255, 255)); // TODO check for bpp

    int width = sdl_screen->w; //screen width
    int height = sdl_screen->h; //screen height
    int middle = height / 2;

    for(int i = 0; i < width; i++) //for each vertical line of pixel
    {
        //angle of the ray in rad
        float ray_angle = (1.0 - i / float(width)) * (player->get_angle() - fov / 2.0) + i / float(width) * (player->get_angle() + fov / 2.0);

        float dist = 0;
        bool hit_wall = false;
        float projectDist = 0;

        //we don't need to calculate sin and cos for each step
        float x_offset = cos(ray_angle);
        float y_offset = sin(ray_angle);

        while(!hit_wall && dist < 40) //makes the ray move forward step by step
        {
            dist += 0.01;
            //pos of the tip of the ray for this iteration
            float ray_x = player->get_x() + x_offset * dist;
            float ray_y = player->get_y() + y_offset * dist;
            //int tile_id = int(ray_x) + int(ray_y) * map->w; //id of current tile

            bool valid = false;
            if(map->get_tile(ushort(ray_x), ushort(ray_y)) == '3') //door
            {
                //std::cout<<sin(float(SDL_GetTicks()) / 1000.0)<<'-';
                if(ray_y - floor(ray_y) < sin(SDL_GetTicks() / 1000.0))
                    valid = true;
            }
            else
                valid = map->get_tile(ushort(ray_x), ushort(ray_y)) != ' '; //the current tile is not empty, we hit a wall

            if(valid)
            {
                hit_wall = true;
                // we need to project the distance onto a flat plane perpendicular to the player
                //in order to avoid fisheye effect
                projectDist = dist * cos(ray_angle - player->get_angle());
                // height of the current vertical segment to draw
                int wall_height = (height / projectDist);

                zbuffer[i] = dist;

                ushort wall_tex = (map->get_tile(ray_x, ray_y) - '0');
                
                ray_x -= floor(ray_x + 0.5);
                ray_y -= floor(ray_y + 0.5);

                // x-texcoord, we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
                bool vertical = std::abs(ray_x) > std::abs(ray_y);
                int texture_x = (vertical ? ray_x : ray_y) * wall_textures->h;
                if(texture_x < 0)
                    texture_x += wall_textures->h;

                int wall_top = middle - wall_height / 2;
                int wall_bottom = wall_top + wall_height;

                if(wall_tex == 2)
                    wall_top += (wall_bottom - wall_top) * 0.2;

                for(int j = 0; j < height; j++)
                {
                    if(j < wall_top) //draw sky (top : 0,50,200 ; bottom : 150,200,200)
                        set_pixel(i, j, rgb_to_int(150 * (j / (float)height * 2), 50 + 150 * (j / (float)height * 2), 200));
                    else if(j >= wall_bottom) //draw ground (top : 0,100,0 ; bottom : 100,200,0)
                        set_pixel(i, j, rgb_to_int(100 - 100 * ((height - j) / (float)height * 2), 200 - 100 * ((height - j) / (float)height * 2), 0));
                    else //top < j < bottom, draw wall
                    {
                        int texture_y = (j - wall_top) / (float)wall_height * wall_textures->h;
                        //displays the pixel and apply lighting
                        set_pixel(i, j, apply_light(get_pixel_tex(wall_tex, texture_x, texture_y), vertical ? 1 : 0.7));
                    }
                }
            }   
        }
    } 

    //Sprites rendering
    map->sort_sprites(player->get_x(), player->get_y());
    std::vector<Sprite> sprites = map->get_sprites();
    for(unsigned int i = 0; i < sprites.size(); i++)
    {
        draw_sprite(sprites.at(i));
    }

    //FPS weapon
    int offset = width / 2 + 100;
    if(player->display_flash)
    {
        draw_2d_sprite(3, offset, height - 400, 400.0);
        player->display_flash = false;
    }
    draw_2d_sprite(0, offset, height - 400, 400.0);

    //draws crosshair
    int h_middle = width / 2;
    set_pixel(h_middle, middle, 0);
    ushort crosshair_size = 8;
    Uint32 crosshair_color = rgb_to_int(0, 255, 255);
    for(int i = 1; i < crosshair_size; i++)
    {
        set_pixel(h_middle+i, middle, crosshair_color);
        set_pixel(h_middle-i, middle, crosshair_color);
        set_pixel(h_middle, middle+i, crosshair_color);
        set_pixel(h_middle, middle-i, crosshair_color);
    }

    SDL_Flip(sdl_screen);
}

void Renderer::draw_sprite(Sprite s)
{
    //direction of the sprite in rad
    float sprite_dir = atan2(-player->get_y() + s.y, s.x - player->get_x());

    //check if the sprite is in the view cone
    if(abs(sprite_dir - player->get_angle()) > fov / 2.0)
        return;

    //calculates sprite size and offset in screen space
    float sprite_dist = sqrt(pow(player->get_x() - s.x, 2) + pow(player->get_y() - s.y, 2));
    int sprite_width = std::min(1000, (int)(1 / sprite_dist * s.size));
    int h_offset = (sprite_dir - player->get_angle()) * sdl_screen->w + (sdl_screen->w / 2) - (sprite_width / 2);
    int v_offset = 360 - (s.size / 2.14);

    //calculates the bouding box of the sprite so it's easier to calculate afterward
    int middle = sdl_screen->h / 2;
    int top = middle - sprite_width / 2 + v_offset / sprite_dist;
    int bottom = middle + sprite_width / 2 + v_offset / sprite_dist;
    float height = bottom - top;

    for(int x = h_offset; x < h_offset + sprite_width; x++)
    {   
        if(sprite_dist < zbuffer[x])
        {
            for(int y = top; y < bottom; y++)
            {
                int texture_x = (x - h_offset) / (float)(sprite_width) * 128;
                int texture_y = (y - top) / (float)height * 128;
                Uint32 pixel = get_pixel_tex(s.itex, texture_x, texture_y, true);

                if(pixel != rgb_to_int(0, 255, 255))
                    set_pixel(x, y, pixel);
            }
        }
    }
}

void Renderer::draw_2d_sprite(ushort itex, ushort x, ushort y, float size)
{
    for(int i = x; i < x + size; i++)
    {
        for(int j = y; j < y + size; j++)
        {
            int texture_x = (i - x) / size * 128;
            int texture_y = (j - y) / size * 128;
            Uint32 pixel = get_pixel_tex(itex, texture_x, texture_y, true);

            if(pixel != rgb_to_int(0, 255, 255))
                set_pixel(i, j, pixel);
        }
    }
}

void Renderer::clean()
{
    if(sdl_screen) SDL_FreeSurface(sdl_screen);
    if(wall_textures) SDL_FreeSurface(wall_textures);
    if(sprites_textures) SDL_FreeSurface(sprites_textures);
    SDL_Quit();
}

//Gets a pixel from the texture file
Uint32 Renderer::get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite)
{
    SDL_Surface* tex = sprite ? sprites_textures : wall_textures;
    if(x >= tex->h || y >= tex->h) 
        return 0;


    Uint8 *p = (Uint8 *)tex->pixels + y * tex->pitch + (x + tex->h * itex) * tex->format->BytesPerPixel;
    return p[0] | p[1] << 8 | p[2] << 16; 
}


//sets a pixel on the screen
void Renderer::set_pixel(ushort x, ushort y, Uint32 pixel)
{
    if (x >= sdl_screen->w || y >= sdl_screen->h)
        return;

    unsigned char bpp = sdl_screen->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)sdl_screen->pixels + y * sdl_screen->pitch + x * bpp;
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