#include "game.h"

int main()
{
    Game game;
    game.init_sdl("stroll stub modif", 1280, 720, 3);
    while (game.running())
    {
        game.handle_events(); // TODO make x_,y_,angle_ increments depend on the dt
        game.draw();          // TODO call draw() 25 times per second only
    }
    game.clean();
    return 0;
}

