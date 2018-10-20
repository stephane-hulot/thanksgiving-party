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
        Renderer(Player* p, Map* m);
        void clean(); // could be moved to the destructor, however shared_ptr would be needed for the member pointers (c11)
        bool init_sdl(const char* title, ushort width, ushort height);
        void draw();
        
    private:
        void set_pixel(ushort x, ushort y, Uint32 pixel);
        Uint32 get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite = false);

        Uint32 rgb_to_int(unsigned char r, unsigned char g, unsigned char b);
        Uint32 apply_light(Uint32 color, float factor);

        void draw_sprite(Sprite s);

        SDL_Surface* sdl_screen;
        SDL_Surface* wall_textures;
        SDL_Surface* sprites_textures;
        float* zbuffer;

        Player* player;
        Map* map;
};

#endif

