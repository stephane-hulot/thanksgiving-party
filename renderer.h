#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"
#include "map.h"

const float fov = 60 * M_PI / 180.0;

const SDL_Color ttf_color_white = {255, 255, 255, 255};
const SDL_Color ttf_color_banana = {209, 182, 6, 255};
const SDL_Color ttf_color_red = {255, 0, 0, 255};

class Renderer
{
    public:
        Renderer(Player* p, Map* m);
        bool init_sdl(const char* title, ushort width, ushort height);
        void draw();
        ~Renderer();

        //copy constructor to avoid warning in c++11
        Renderer(const Renderer& r) : Renderer(r.player, r.map) {};
        Renderer& operator=(Renderer r);
        
    private:
        void set_pixel(ushort x, ushort y, Uint32 pixel);
        Uint32 get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite = false);

        Uint32 rgb_to_int(unsigned char r, unsigned char g, unsigned char b);
        Uint32 apply_light(Uint32 color, float factor);

        void draw_sprite(Sprite s);
        void draw_2d_sprite(ushort itex, ushort x, ushort y, float size);
        void draw_text(ushort x, ushort y, std::string text, bool big_text, SDL_Color ttf_color);

        void menu();
        void pause_menu();
        void game_over();

        SDL_Window* window;
        SDL_Renderer* sdl_renderer;
        SDL_Texture* render_texture;
        Uint32* pixels;
        ushort screen_w;
        ushort screen_h;

        SDL_Surface* wall_textures;
        SDL_Surface* sprites_textures;
        TTF_Font *font_big;
        TTF_Font *font_medium;
        float* zbuffer;

        Player* player;
        Map* map;
};

#endif

