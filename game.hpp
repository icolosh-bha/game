// game.hpp
#ifndef GAME_HPP
#define GAME_HPP

#include <C:\Users\hangu\Downloads\SDL2-devel-2.28.5-mingw\SDL2-2.28.5\x86_64-w64-mingw32\include\SDL2\SDL.h>
#include "map.h"

// Forward declaration của lớp Map
class Map;

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }
    static SDL_Renderer* renderer; // Giữ lại biến static renderer

private:
    bool isRunning;
    SDL_Window* window;
    // Xóa biến SDL_Renderer* renderer ở đây để tránh xung đột
    SDL_Texture* playerTexture;
    SDL_Rect playerRect;
    Map* gameMap; // Sử dụng con trỏ để tránh circular dependency
     bool hasKey;
    void showMessage(const char* message);
};

#endif
