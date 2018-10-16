#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <cmath>
#include <SDL/SDL.h>
#include "player.h"
#include "map.h"

const float fov = 60 * M_PI / 180.0;

class Renderer
{
    public:
        Renderer(Player* p);
        //void display_framerate(ushort fps)
        void clean(); // could be moved to the destructor, however shared_ptr would be needed for the member pointers (c11)
        bool init_sdl(const char* title, ushort width, ushort height, unsigned char bpp);        void draw();
        
    private:
        void set_pixel(ushort x, ushort y, Uint32 pixel);
        Uint32 get_pixel_tex(ushort itex, ushort x, ushort y);

        Uint32 rgb_to_int(unsigned char r, unsigned char g, unsigned char b);
        Uint32 apply_light(Uint32 color, float factor);

        void draw_sprite(ushort itex, ushort sprite_x, ushort sprite_y, ushort sprite_size);

        SDL_Surface* sdl_screen_;
        SDL_Surface* textures_;
        int ntextures;
        int texsize;
        float* zbuffer;

        Player* player;
};

#endif

