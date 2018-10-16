#include <ctime>
#include <iostream>
#include "renderer.h"
#include "player.h"

int main()
{
    uint lastTime = 0;

	Player player;
    Renderer* renderer = new Renderer(&player);
    renderer->init_sdl("Thanksgiving Party", 1280, 720, 3);
              
    while (!player.wants_to_quit)
    {
        player.handle_events(); // TODO make x_,y_,angle_ increments depend on the dt
        renderer->draw();          // TODO call draw() 25 times per second only

        ushort dt = SDL_GetTicks() - lastTime;
        int fps = 1000 / dt;
        std::cout<<fps<<" FPS"<<std::endl;
        //renderer->display_framerate(fps);
        lastTime = SDL_GetTicks();
    }
    renderer->clean();
    return 0;
}

