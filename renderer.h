#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"

const float fov = 60 * M_PI / 180.0;

const SDL_Color ttf_color_white = {255, 255, 255, 255};
const SDL_Color ttf_color_banana = {209, 182, 6, 255};
const SDL_Color ttf_color_red = {255, 0, 0, 255};

class Renderer
{
    public:
        Renderer(Player* p, Map* ma, Menu* me);
        bool init_sdl(const char* title, ushort width, ushort height); //initializes SDL and necessary data for rendering
        void draw(uint fps); //renders the game and UI and displays it
        ~Renderer();

        //forbids copy constructor to avoid warning in c++11
        Renderer(const Renderer& r) = delete;
        Renderer& operator=(const Renderer& r) = delete;
        
    private:
        void set_pixel(ushort x, ushort y, Uint32 pixel); //sets a pixel on the screen
        Uint32 get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite = false); //gets a pixel from the texture file

        Uint32 rgb_to_int(unsigned char r, unsigned char g, unsigned char b); //convert an RGB value to a Uint32 used by SDL
        Uint32 apply_light(Uint32 color, float factor); //darkens a color using a factor, 1 = no change, 0 = black

        void draw_sprite(Sprite s); //draws a sprite in the world
        void draw_2d_sprite(ushort itex, ushort x, ushort y, float size); //draws a sprite as a flat texture on the screen 
        void draw_text(ushort x, ushort y, std::string text, bool big_text, SDL_Color ttf_color); //draws text using sdl_ttf

        //UI display
        void display_normal(int fps);
        void display_menu();
        void display_pause_menu();
        void display_game_over();
        void display_win();
        void display_help();

        SDL_Window* window;
        SDL_Renderer* sdl_renderer;
        SDL_Texture* render_texture;

        Uint32* pixels; //all the pixels rendered on screen
        ushort screen_w; //screen width
        ushort screen_h; //screen height

        SDL_Surface* wall_textures;
        SDL_Surface* sprites_textures;
        TTF_Font *font_big; //font with a size of 100
        TTF_Font *font_medium; //font with a size of 60
        float* zbuffer; //array containing all distances

        Player* player;
        Map* map;
        Menu* menu;
};

#endif

