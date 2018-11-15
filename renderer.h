#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"
#include "map.h"

const float fov = 60 * M_PI / 180.0;

class Renderer
{
    public:
        Renderer(Player* p, Map* m);
        bool init_sdl(const char* title, ushort width, ushort height);
        void draw(int fps);
        ~Renderer();

        //copy constructor to avoid warning in c++11
        Renderer(const Renderer&);
        Renderer& operator=(Renderer r);
        
    private:
        void set_pixel(ushort x, ushort y, Uint32 pixel);
        Uint32 get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite = false);

        Uint32 rgb_to_int(unsigned char r, unsigned char g, unsigned char b);
        Uint32 apply_light(Uint32 color, float factor);

        void draw_sprite(Sprite s);
        void draw_2d_sprite(ushort itex, ushort x, ushort y, float size);
        void draw_text(ushort x, ushort y, std::string text, ushort font_size);

        SDL_Window* window;
        SDL_Renderer* sdl_renderer;
        SDL_Texture* render_texture;
        Uint32* pixels;
        ushort screen_w;
        ushort screen_h;

        SDL_Surface* wall_textures;
        SDL_Surface* sprites_textures;
        float* zbuffer;

        Player* player;
        Map* map;
};

#endif

