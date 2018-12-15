#include <iostream>
#include <cmath>
#include "renderer.h"

Renderer::Renderer(Player* p, Map* ma, Menu* me) : window(NULL), sdl_renderer(NULL), render_texture(NULL), pixels(NULL), screen_w(0), screen_h(0),
    wall_textures(NULL), sprites_textures(NULL), font_big(NULL), font_medium(NULL), zbuffer(NULL), player(p), map(ma), menu(me)
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

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    sdl_renderer = SDL_CreateRenderer(window, -1, 0);
    render_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    screen_w = width;
    screen_h = height;
    pixels = new Uint32[screen_w * screen_h];

    if(!window || !sdl_renderer || !render_texture)
    {
        std::cerr << "SDL Renderer init failed, SDL_GetError()=" << SDL_GetError() << std::endl;
        return false;
    }

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

    zbuffer = new float[screen_w];
    
    TTF_Init();
    font_big = TTF_OpenFont("pixelz.ttf", 100);
    font_medium = TTF_OpenFont("pixelz.ttf", 60);
    if(!font_big || !font_medium)
    {
        std::cerr << "Couldn't load ttf file " << SDL_GetError() << std::endl;
        return false;
    }
    
    return true;
}

//##########
//  DRAW
//##########

void Renderer::draw(uint fps)
{
    int middle = screen_h / 2;

    for(int i = 0; i < screen_w; i++) //for each vertical column of pixel
    {
        //angle of the ray in rad
        float ray_angle = (1.0 - i / float(screen_w)) * (player->get_angle() - fov / 2.0) + i / float(screen_w) * (player->get_angle() + fov / 2.0);

        float dist = 0;
        bool hit_wall = false;
        float projectDist = 0;

        //we don't need to calculate sin and cos for each step
        float x_offset = cos(ray_angle);
        float y_offset = sin(ray_angle);

        while(!hit_wall && dist < 40) //makes the ray move forward step by step
        {
            dist += 0.02;
            //pos of the tip of the ray for this iteration
            float ray_x = player->get_x() + x_offset * dist;
            float ray_y = player->get_y() + y_offset * dist;

            bool valid = false;
            if(map->get_tile(ushort(ray_x), ushort(ray_y)) == '3') //door
            {
                Door door = map->get_door(ushort(ray_x), ushort(ray_y));
                if(ray_y - floor(ray_y) < door.animationState)
                    valid = true;
            }
            else
                valid = map->get_tile(ushort(ray_x), ushort(ray_y)) != ' '; //the current tile is not empty, we hit a wall

            if(valid)
            {
                hit_wall = true;
                ushort tile_id = map->get_tile(ushort(ray_x), ushort(ray_y)) - '0';

                // we need to project the distance onto a flat plane perpendicular to the player
                //in order to avoid fisheye effect
                projectDist = dist * cos(ray_angle - player->get_angle());
                // height of the current vertical segment to draw
                int wall_height = (screen_h / projectDist) * 1.2;

                zbuffer[i] = dist;
                
                float local_ray_x = ray_x - floor(ray_x + 0.5);
                float local_ray_y = ray_y - floor(ray_y + 0.5);

                // x-texcoord, we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
                bool vertical = std::abs(local_ray_x) > std::abs(local_ray_y) && tile_id != 3;
                int texture_x = (vertical ? local_ray_x : local_ray_y) * wall_textures->h;
                if(texture_x < 0)
                    texture_x += wall_textures->h;

                if(tile_id == 3)
                {
                    Door d = map->get_door(ushort(ray_x), ushort(ray_y));
                    int offset = wall_textures->h - d.animationState * wall_textures->h;
                    texture_x += offset;
                }

                int wall_top = middle - wall_height / 2;
                int wall_bottom = wall_top + wall_height;

                if(tile_id == 2)
                    wall_top += (wall_bottom - wall_top) * 0.2;

                for(int j = 0; j < screen_h; j++)
                {
                    if(j < wall_top) //draw sky (top : 0,50,200 ; bottom : 150,200,200)
                        set_pixel(i, j, rgb_to_int(150 * (j / (float)screen_h * 2), 50 + 150 * (j / (float)screen_h * 2), 200));
                    else if(j >= wall_bottom) //draw ground (top : 0,100,0 ; bottom : 100,200,0)
                        set_pixel(i, j, rgb_to_int(120 - 70 * ((screen_h - j) / (float)screen_h * 2), 150 - 50 * ((screen_h - j) / (float)screen_h * 2), 20));
                    else //top < j < bottom, draw wall
                    {
                        int texture_y = (j - wall_top) / (float)wall_height * wall_textures->h;
                        //displays the pixel and apply lighting
                        set_pixel(i, j, apply_light(get_pixel_tex(tile_id, texture_x, texture_y), vertical ? 1 : 0.7));
                    }
                }
            }   
        }
    } 

    //Sprites rendering
    std::vector<Sprite> sprites = map->get_sprites();
    for(unsigned int i = 0; i < sprites.size(); i++)
    {
        draw_sprite(sprites.at(i));
    }

    if(menu->current == None)
    {
        //FPS weapon
        int offset = screen_w / 2 + 100;
        if(player->display_flash)
        {
            draw_2d_sprite(3, offset, screen_h - 400, 400.0);
        }
        draw_2d_sprite(0, offset, screen_h - 400, 400.0);

        //draws crosshair
        int center = screen_w / 2;
        middle += 30;
        ushort crosshair_size = 8;
        Uint32 crosshair_color = rgb_to_int(0, 255, 255);
        for(int i = 3; i < crosshair_size; i++)
        {
            set_pixel(center+i, middle, crosshair_color);
            set_pixel(center-i, middle, crosshair_color);
            set_pixel(center, middle+i, crosshair_color);
            set_pixel(center, middle-i, crosshair_color);
        }

        //health bar
        Uint32 color = rgb_to_int(200, 30, 30);
        Uint32 bg_color = rgb_to_int(30, 0, 0);
        int hp_width = (player->health * 2.56) + 8;
        for(int i = screen_h - 58; i < screen_h - 8; i++)
        {
            for(int j = 8; j < 8 + 256; j++)
            {
                set_pixel(j, i, j < hp_width ? color : bg_color);
            }
        }

        //keys
        for(int i = 0; i < player->key_count; i++)
        {
            draw_2d_sprite(5, 10 + i * 100, screen_h - 150, 100);
        }
    }

    if(menu->current == Main)
        draw_2d_sprite(1, 500, 140, 200);
    
    SDL_UpdateTexture(render_texture, NULL, pixels, screen_w * sizeof(Uint32));
    SDL_RenderCopy(sdl_renderer, render_texture, NULL, NULL);

    if(menu->current == Main)
        display_menu();
    else if(menu->current == Pause)
        display_pause_menu();
    else if(menu->current == GameOver)
        display_game_over();
    else if(menu->current == Win)
        display_win();
    else if(menu->current == Help)
        display_help();
    else
        display_normal(fps);
    
    SDL_RenderPresent(sdl_renderer);
}

//##########
//  SPRITES
//##########

void Renderer::draw_sprite(Sprite s)
{
    //direction of the sprite in rad
    float sprite_dir = atan2(-player->get_y() + s.y, s.x - player->get_x());    
    if(sprite_dir - player->get_angle() > M_PI)
        sprite_dir -= 2 * M_PI;
    if(sprite_dir - player->get_angle() < -M_PI)
        sprite_dir += 2 * M_PI;

    //check if the sprite is in the view cone
    if(abs(sprite_dir - player->get_angle()) > fov / 2.0)
        return;

    //calculates sprite size and offset in screen space
    float sprite_dist = sqrt(pow(player->get_x() - s.x, 2) + pow(player->get_y() - s.y, 2));
    int sprite_width = std::min(1000, (int)(1 / sprite_dist * s.size));
    int h_offset = (sprite_dir - player->get_angle()) * screen_w + (screen_w / 2) - (sprite_width / 2);
    int v_offset = 400 - (s.size / 3);

    //calculates the bouding box of the sprite so it's easier to calculate afterward
    int middle = screen_h / 2;
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

//##########
//  UI
//##########

void Renderer::draw_text(ushort x, ushort y, std::string text, bool big_text, SDL_Color ttf_color)
{
    SDL_Surface *ttf_surface;
    SDL_Texture *ttf_texture;
    
    ttf_surface = TTF_RenderText_Solid(big_text ? font_big : font_medium, text.c_str(), ttf_color);
    ttf_texture = SDL_CreateTextureFromSurface(sdl_renderer, ttf_surface);

    int w, h;
    SDL_QueryTexture(ttf_texture, NULL, NULL, &w, &h);

    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    pos.w = w;
    pos.h = h;

    SDL_RenderCopy(sdl_renderer, ttf_texture, NULL, &pos);

    SDL_DestroyTexture(ttf_texture);
    SDL_FreeSurface(ttf_surface);
}

void Renderer::display_normal(int fps)
{   
    std::string fps_text = std::to_string(fps) + " FPS";
    std::string score_text = std::to_string(map->enemy_count) + " turkeys left - Timer:" + menu->timer.get_time_string();
    draw_text(10, 10, score_text, false, ttf_color_white);
    draw_text(10, 60, fps_text, false, ttf_color_white);
    draw_text(10, screen_h - 62, std::to_string(player->health), false, ttf_color_white);
}

void Renderer::display_menu()
{   
    draw_text(100, 50, "Thanksgiving Party", true, ttf_color_white);
    draw_text(550, 350, "PLAY", false, menu->check_hover(0) ? ttf_color_banana : ttf_color_white);
    std::string diff_display = std::string("DIFFICULTY:") + (menu->difficulty == 0 ? "EASY" : (menu->difficulty == 1 ? "NORMAL" : "HARD"));
    draw_text(380, 420, diff_display, false, menu->check_hover(1) ? ttf_color_banana : ttf_color_white);
    std::string sound_display = std::string("SOUND:") + (menu->sound == 0 ? "NONE" : (menu->sound == 1 ? "LOW" : "NORMAL"));
    draw_text(460, 490, sound_display, false, menu->check_hover(8) ? ttf_color_banana : ttf_color_white);
    draw_text(550, 560, "HELP", false, menu->check_hover(5) ? ttf_color_banana : ttf_color_white);
    draw_text(550, 620, "QUIT", false, menu->check_hover(2) ? ttf_color_banana : ttf_color_white);
}

void Renderer::display_pause_menu()
{   
    draw_text(470, 100, "PAUSE", true, ttf_color_white);
    draw_text(520, 300, "RESUME", false, menu->check_hover(3) ? ttf_color_banana : ttf_color_white);
    draw_text(550, 370, "QUIT", false, menu->check_hover(4) ? ttf_color_banana : ttf_color_white);
}

void Renderer::display_game_over()
{   
    draw_text(380, 200, "GAME OVER", true, ttf_color_red);
}

void Renderer::display_win()
{   
    draw_text(380, 50, "You won !", true, ttf_color_white);
    draw_text(250, 170, "Your time : " + menu->timer.get_time_string(), false, ttf_color_white);
    for(int i = 0; i < 5; i++)
    {
        std::string text = std::to_string(i+1) + " - ";
        text += menu->timer.get_time_string(menu->leaderboard.scores[i]);
        draw_text(300, 250 + i * 70, text, false, ttf_color_white);
    }
    draw_text(550, 600, "QUIT", false, menu->check_hover(7) ? ttf_color_banana : ttf_color_white);
}

void Renderer::display_help()
{   
    draw_text(500, 50, "HELP", true, ttf_color_white);
    draw_text(20, 200, "Move : Z/Q/S/D", false, ttf_color_white);
    draw_text(20, 270, "Camera : Left and right arrows", false, ttf_color_white);
    draw_text(20, 340, "Fire : Spacebar", false, ttf_color_white);
    draw_text(20, 450, "Kill every turkey as fast ", false, ttf_color_white);
    draw_text(20, 520, "as possible !", false, ttf_color_white);
    draw_text(530, 600, "BACK", false, menu->check_hover(6) ? ttf_color_banana : ttf_color_white);
}

//##########
//  DRAW UTILS
//##########

Uint32 Renderer::get_pixel_tex(ushort itex, ushort x, ushort y, bool sprite)
{
    SDL_Surface* tex = sprite ? sprites_textures : wall_textures;
    if(x >= tex->h || y >= tex->h) 
        return 0;

    Uint8 *p = (Uint8 *)tex->pixels + y * tex->pitch + (x + tex->h * itex) * tex->format->BytesPerPixel;
    return p[0] | p[1] << 8 | p[2] << 16; 
}

void Renderer::set_pixel(ushort x, ushort y, Uint32 pixel)
{
    if (x >= screen_w || y >= screen_h)
        return;
    
    pixels[x + screen_w * y] = pixel;
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

//##########

Renderer::~Renderer()
{
    if(render_texture) SDL_DestroyTexture(render_texture);
    if(sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
    if(window) SDL_DestroyWindow(window);
    if(wall_textures) SDL_FreeSurface(wall_textures);
    if(sprites_textures) SDL_FreeSurface(sprites_textures);
    if(font_big) TTF_CloseFont(font_big);
    if(font_medium) TTF_CloseFont(font_medium);
    delete pixels;
    delete zbuffer;

    TTF_Quit();
    SDL_Quit();

    std::cout<<"Renderer deleted"<<std::endl;
}