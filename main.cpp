// main.cpp
#include <iostream>
#include <C:\Users\hangu\Downloads\SDL2-devel-2.28.5-mingw\SDL2-2.28.5\x86_64-w64-mingw32\include\SDL2\SDL.h>
#include "Game.hpp"

int main(int argc, char* argv[]) {
    const int FPS = 60;
    const int framDelay = 1000 / FPS;
    Uint32 framStart;
    int framtime;
    Game game;
    game.init("SDL2 Texture Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
    while (game.running()) {
        framStart = SDL_GetTicks();
        game.handleEvents();
        game.update();
        game.render();
        framtime = SDL_GetTicks() - framStart;
        if (framDelay > framtime) {
            SDL_Delay(framDelay - framtime);
        }
    }
    game.clean();
    return 0;
}
