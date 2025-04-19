#pragma once
#include "game.hpp"

#include <SDL.h>

class Map {
public:
    Map();
    ~Map();

    void Loadmap();
    void Drawmap();
    int getTileAt(int row, int col);
    void UpdateTile(int row, int col, int newValue); // cập nhật 1 ô
    void RevealTrap(int row, int col); // New method to reveal trap

private:
    SDL_Rect src, dest;
    SDL_Texture* wall;
    SDL_Texture* dirt;
    SDL_Texture* cloudy;
    SDL_Texture* trap;
    SDL_Texture* key;
    SDL_Texture* goal;
    int map[20][25];
    bool discoveredTraps[20][25] = {false}; // Track discovered traps
};
