#pragma once
#include <SDL.h>
#include "game.hpp"
#include "textureManager.h"

// Forward declaration
class Map;

class Monster {
public:
    Monster(Map* map);
    ~Monster();

    void update(int playerX, int playerY);
    void render();
    SDL_Rect* getRect();
    void moveTowardsPlayer(int playerX, int playerY);

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    Map* gameMap;
    static const int SPEED = 1;
    static const int TILE_SIZE = 32;
};
