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
        void clean(); // could be moved to the destructor, however shared_ptr would be needed for the member pointers (c11)
        bool init_sdl(const char* title, int width, int height, int bpp);
        void draw();
        
    private:
        void set_pixel(int x, int y, Uint32 pixel);
        Uint32 get_pixel_tex(int itex, int x, int y);

        Uint32 rgb_to_int(int r, int g, int b);
        Uint32 apply_light(Uint32 color, float factor);

        SDL_Surface* sdl_screen_;
        SDL_Surface* textures_;
        int ntextures;
        int texsize;

        Player* player;
};

#endif

