#pragma once
#include <SDL.h>

class Map;

class Monster {
public:
    Monster(Map* map);
    void update(int playerX, int playerY);
    SDL_Rect* getRect();
    void render();
    void moveTowardsPlayer(int playerX, int playerY);
    void setKeyStatus(bool hasKey); // Thêm khai báo hàm này

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    Map* gameMap;
    static const int TILE_SIZE = 32;
    bool isActive; // Thêm biến này để lưu trạng thái
};
