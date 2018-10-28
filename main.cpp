#include <ctime>
#include <iostream>
#include "renderer.h"
#include "player.h"

int main()
{
    uint lastTime = 0;
    uint lastFPSDisplay = 0;

    Map map;
	Player* player = new Player(&map);
    Renderer* renderer = new Renderer(player, &map);

    if(!renderer->init_sdl("Thanksgiving Party", 1280, 720))
        return 0;
              
    while (!player->wants_to_quit)
    {
        float dt = (SDL_GetTicks() - lastTime) / 1000.0;
        lastTime = SDL_GetTicks();
        if(dt == 0) dt = 1; //prevent division by zero

        if(lastFPSDisplay + 200 < lastTime)
        {
            lastFPSDisplay = lastTime;
            int fps = 1 / dt;
            std::cout<<fps<<" FPS"<<std::endl;
        }

        map.update_ai(player->get_x(), player->get_y());
        player->handle_events(dt);
        renderer->draw();
    }
    
    renderer->clean();
    return 0;
}

