#pragma once
#include <SDL.h>
#include <C:\Users\hangu\Downloads\SDL2_image-devel-2.8.4-mingw\SDL2_image-2.8.4\x86_64-w64-mingw32\include\SDL2\SDL_image.h>
#include "textureManager.h"
#include "game.hpp"

class Map {
public:
    Map();
    ~Map();

    void Loadmap();
    void Drawmap();
    int getTileAt(int row, int col);
    void UpdateTile(int row, int col, int newValue); // cập nhật 1 ô
    void setShowTraps(bool show) {
        if (show && !trap) {
            trap = TextureManager::LoadTexture("trap.png");
        }
        showTraps = show;
    }
    bool isShowingTraps() const { return showTraps; }

private:
    SDL_Rect src, dest;
    SDL_Texture* wall;
    SDL_Texture* dirt;
    SDL_Texture* cloudy;
    SDL_Texture* trap = nullptr;  // Initialize to nullptr
    SDL_Texture* key;
    SDL_Texture* goal;
    int map[21][25];
    bool showTraps = false;  // New variable to control trap visibility
};
