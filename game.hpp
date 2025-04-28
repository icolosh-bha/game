#pragma once
#include <SDL.h>
#include <SDL2/SDL_ttf.h>

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
    void resetPlayerPosition();  // New function to reset player position
    void removeTrapNearPosition(int x, int y);  // New function to remove nearest trap
    void initFont(); // Thêm khai báo hàm initFont

    bool running() { return isRunning; }
    static SDL_Renderer* renderer; // Giữ lại biến static renderer

private:
    bool isRunning;
    SDL_Window* window;
    SDL_Texture* playerTexture;
    SDL_Texture* keyTexture;
    SDL_Texture* goalTexture;
    SDL_Texture* trapTexture;
    SDL_Texture* wallTexture;
    TTF_Font* defaultFont;
    SDL_Rect playerRect;
    Map* gameMap;
    Monster* monster;
    bool hasKey;
    bool monsterActive;
    int lives;
    int initialPlayerX;
    int initialPlayerY;
    int initialMonsterX;
    int initialMonsterY;
    void showMessage(const char* message);
    bool showConfirmMessage(const char* message);
};
