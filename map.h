#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>

class Map {
public:
    Map();
    ~Map();

    void Loadmap(); // Nếu dùng hardcoded (cũ)
    bool loadFromFile(const char* filename); // Mới: đọc bản đồ từ file

    void Drawmap();
    int getTileAt(int row, int col);
    void UpdateTile(int row, int col, int newValue); // cập nhật 1 ô

    void setShowTraps(bool show) { showTraps = show; }
    bool isShowingTraps() const { return showTraps; }

    void revealTrap(int row, int col);
    bool isTrapRevealed(int row, int col) const;
 int map[21][25];
private:
    SDL_Rect src, dest;

    SDL_Texture* wall = nullptr;
    SDL_Texture* dirt = nullptr;
    SDL_Texture* cloudy = nullptr;
    SDL_Texture* trap = nullptr;
    SDL_Texture* key = nullptr;
    SDL_Texture* goal = nullptr;

    bool showTraps = false;  // Điều khiển hiện trap
    std::vector<std::pair<int, int>> revealedTraps; // Trap đã hiện



};
