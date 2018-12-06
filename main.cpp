#include <ctime>
#include <iostream>
#include "renderer.h"
#include "player.h"

int main()
{
    uint lastTime = 0;
    uint lastFPSDisplay = 0;
    uint fps = 60;

    Map map;
    if(map.w == 0) return 0;
    Menu menu;
	Player player(&map, &menu);
    Renderer renderer(&player, &map, &menu);

    if(!renderer.init_sdl("Thanksgiving Party", 1280, 720))
        return 0;
    
    //game loop
    while (!menu.wants_to_quit)
    {
        float dt = (SDL_GetTicks() - lastTime) / 1000.0;
        lastTime = SDL_GetTicks();
        if(dt == 0) dt = 1; //prevent division by zero

        //called five times per second
        if(lastFPSDisplay + 200 < lastTime)
        {
            lastFPSDisplay = lastTime;
            fps = 1 / dt;

            //sort sprites only 5 times per seconds
            map.sort_sprites(player.get_x(), player.get_y());
            map.animate_sprites();

            if(map.pickup_keys())
                player.key_count++;

            if(menu.current == None)
                player.health -= map.damage_player();
        }
        
        if(menu.current == None)
        {
            if(player.key_count > 0 && map.update_doors(player.get_x(), player.get_y(), dt))
                player.key_count--;

            map.update_sprites(player.get_x(), player.get_y(), dt);
        }
        else if(menu.current == Main)
        {
            map.damage = menu.difficulty == 0 ? 1 : (menu.difficulty == 1 ? 3 : 6);
            map.speed = menu.difficulty == 0 ? 0.8 : (menu.difficulty == 1 ? 1.2 : 2.5);
        }

        player.handle_events(dt);
        renderer.draw(fps);
    }

    return 0;
}

