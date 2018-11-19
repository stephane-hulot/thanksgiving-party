#include <iostream>
#include <cmath>
#include "renderer.h"

Renderer::Renderer(Player* p, Map* m) : window(NULL), sdl_renderer(NULL), render_texture(NULL), pixels(NULL),
    screen_w(0), screen_h(0), wall_textures(NULL), sprites_textures(NULL), zbuffer(NULL), player(p), map(m)
{
    
}

Renderer::Renderer(const Renderer& r) : window(NULL), sdl_renderer(NULL), render_texture(NULL), pixels(NULL),
    screen_w(0), screen_h(0), wall_textures(NULL), sprites_textures(NULL), zbuffer(NULL), player(NULL), map(NULL)
{
    player = r.player;
    map = r.map;
}

Renderer& Renderer::operator=(Renderer r)
{
    player = r.player;
    map = r.map;
    return *this;
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

    zbuffer = new float[screen_w];

    TTF_Init();
    return true;
}

void Renderer::draw(int fps)
{
    int middle = screen_h / 2;

    for(int i = 0; i < screen_w; i++) //for each vertical line of pixel
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

    if(!player->menu)
    {
        //FPS weapon
        int offset = screen_w / 2 + 100;
        if(player->display_flash)
        {
            draw_2d_sprite(3, offset, screen_h - 400, 400.0);
            player->display_flash = false;
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
    }
    else
        draw_2d_sprite(1, 500, 200, 200);
    
    SDL_UpdateTexture(render_texture, NULL, pixels, screen_w * sizeof(Uint32));
    SDL_RenderCopy(sdl_renderer, render_texture, NULL, NULL);

    if(player->menu)
    {
        draw_text(100, 100, "Thanksgiving Party", 100);
    }
    else
    {   
        std::string score_text = "Score " + std::to_string(player->score)
            + " - Health : " + std::to_string(player->health)
            + " - " + std::to_string(fps) + "FPS"; 
        draw_text(10, 10, score_text, 40);
    }
    
    SDL_RenderPresent(sdl_renderer);

}

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

void Renderer::draw_text(ushort x, ushort y, std::string text, ushort font_size)
{
    SDL_Surface *ttf_surface;
    SDL_Texture *ttf_texture;
    TTF_Font *font = NULL;
    SDL_Color ttf_color = {255, 255, 255, 255};

    font = TTF_OpenFont("pixelz.ttf", font_size);
    ttf_surface = TTF_RenderText_Solid(font, text.c_str(), ttf_color);
    ttf_texture = SDL_CreateTextureFromSurface(sdl_renderer, ttf_surface);

    int w, h;
    SDL_QueryTexture(ttf_texture, NULL, NULL, &w, &h);

    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    pos.w = w;
    pos.h = h;

    SDL_RenderCopy(sdl_renderer, ttf_texture, NULL, &pos);

    TTF_CloseFont(font);
    SDL_DestroyTexture(ttf_texture);
    SDL_FreeSurface(ttf_surface);
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

Renderer::~Renderer()
{
    if(render_texture) SDL_DestroyTexture(render_texture);
    if(sdl_renderer) SDL_DestroyRenderer(sdl_renderer);
    if(window) SDL_DestroyWindow(window);
    if(wall_textures) SDL_FreeSurface(wall_textures);
    if(sprites_textures) SDL_FreeSurface(sprites_textures);
    delete pixels;
    delete zbuffer;

    TTF_Quit();
    SDL_Quit();

    std::cout<<"Renderer deleted"<<std::endl;
}