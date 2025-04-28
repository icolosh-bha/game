#pragma once
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include "textureManager.h"
#include "game.hpp"
#include <vector>
#include <utility>

class Map {
public:
    Map();
    ~Map();

    void Loadmap();
    void Drawmap();
    int getTileAt(int row, int col);
    void UpdateTile(int row, int col, int newValue); // cập nhật 1 ô
    void setShowTraps(bool show) {
        showTraps = show;
    }
    bool isShowingTraps() const { return showTraps; }

    void revealTrap(int row, int col);
    bool isTrapRevealed(int row, int col) const;

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
    std::vector<std::pair<int, int>> revealedTraps; // Vector lưu các trap đã reveal
};
