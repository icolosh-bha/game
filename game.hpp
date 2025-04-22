// game.hpp
#pragma once
#include <SDL.h>

// Forward declarations
class Map;
class Monster;

class Game {
public:
    static const int SCREEN_WIDTH = 800;  // Thêm kích thước màn hình
    static const int SCREEN_HEIGHT = 640;
    static const int TILE_SIZE = 32;      // Thêm kích thước tile

    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }
    static SDL_Renderer* renderer; // Giữ lại biến static renderer
    bool hasKey;
    bool monsterActive;
    bool gameLost;
    bool gameWon;

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Texture* playerTexture;
    SDL_Texture* keyTexture;
    SDL_Texture* goalTexture;  // Thêm texture cho goal
    SDL_Texture* trapTexture;  // Thêm texture cho trap
    SDL_Rect playerRect;
    Map* gameMap; // Sử dụng con trỏ để tránh circular dependency
    Monster* monster;

    void showMessage(const char* message);
};
