#include "map.h"
#include "textureManager.h"
#include "game.hpp"
#include <iostream>
#include <vector> // Thêm thư viện vector để sử dụng std::vector

int lvl1[21][25] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,1,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,5,2},
    {2,2,2,0,2,0,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2},
    {2,0,0,0,0,0,0,2,0,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,2},
    {2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,3,2},
    {2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2},
    {2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,3,2,0,0,0,2,0,0,0,2},
    {2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2},
    {2,0,3,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2},
    {2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2},
    {2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,2},
    {2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2,0,2,2,2},
    {2,0,0,0,0,0,0,2,0,0,0,3,0,0,2,0,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,4,2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2}
};

Map::Map() {
    std::cout << "Initializing Map..." << std::endl;

    wall = TextureManager::LoadTexture("wall.png");
    if (!wall) std::cout << "Failed to load wall.png" << std::endl;

    dirt = TextureManager::LoadTexture("dirt.png");
    if (!dirt) std::cout << "Failed to load dirt.png" << std::endl;

    cloudy = TextureManager::LoadTexture("cloudy.png");
    if (!cloudy) std::cout << "Failed to load cloudy.png" << std::endl;

    key = TextureManager::LoadTexture("key.png");
    if (!key) std::cout << "Failed to load key.png" << std::endl;

    goal = TextureManager::LoadTexture("goal.png");
    if (!goal) {
        std::cout << "Failed to load goal.png" << std::endl;
    } else {
        std::cout << "Successfully loaded goal.png" << std::endl;
    }

    trap = TextureManager::LoadTexture("trap.png");  // Load trap texture at initialization
    if (!trap) {
        std::cout << "Failed to load trap.png" << std::endl;
    } else {
        std::cout << "Successfully loaded trap.png" << std::endl;
    }

    src.x = src.y = 0;
    src.w = dest.w = 32;
    src.h = dest.h = 32;
    dest.x = dest.y = 0;

    Loadmap();
    showTraps = false;  // Start with traps hidden
    revealedTraps.clear(); // Clear revealedTraps vector
}

Map::~Map() {
    SDL_DestroyTexture(wall);
    SDL_DestroyTexture(dirt);
    SDL_DestroyTexture(cloudy);
    SDL_DestroyTexture(trap);
    SDL_DestroyTexture(key);
    SDL_DestroyTexture(goal);
}

void Map::Loadmap() {
    for (int row = 0; row < 21; row++) {
        for (int col = 0; col < 25; col++) {
            map[row][col] = lvl1[row][col];
        }
    }
}

int Map::getTileAt(int row, int col) {
    if (row < 0 || row > 20 || col < 0 || col > 25) return -1;
    return map[row][col];
}

void Map::UpdateTile(int row, int col, int newValue) {
    if (row >= 0 && row <= 20 && col >= 0 && col <= 25) {
        map[row][col] = newValue;
    }
}

void Map::Drawmap() {
    int type = 0;

    for (int row = 0; row < 21; row++) {
        for (int col = 0; col < 25; col++) {
            type = map[row][col];

            dest.x = col * 32;
            dest.y = row * 32;
            dest.w = 32;
            dest.h = 32;

            switch (type) {
            case 0:     // Đường đi
                SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(Game::renderer, &dest);
                break;
            case 1:     // Path
                SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(Game::renderer, &dest);
                break;
            case 2:     // Wall
                if (wall) {
                    SDL_RenderCopy(Game::renderer, wall, &src, &dest);
                } else {
                    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                }
                break;
            case 3:     // Trap
                SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                SDL_RenderFillRect(Game::renderer, &dest);

                if ((showTraps || isTrapRevealed(row, col)) && trap) {
                    SDL_RenderCopy(Game::renderer, trap, &src, &dest); // Hiện trap đã reveal hoặc khi showTraps = true
                } else if (wall) {
                     SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(Game::renderer, &dest);
                }
                break;
            case 4:     // Goal
                if (goal) {
                    SDL_RenderCopy(Game::renderer, goal, &src, &dest);
                } else {
                    SDL_SetRenderDrawColor(Game::renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                }
                break;
            case 5:     // Key
                SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 255);
                SDL_RenderFillRect(Game::renderer, &dest);
                break;
            default:
                break;
            }
        }
    }
}

void Map::revealTrap(int row, int col) {
    if (!isTrapRevealed(row, col)) {
        revealedTraps.push_back({row, col});
        std::cout << "Revealed trap at position: " << row << "," << col << std::endl;
    }
}

bool Map::isTrapRevealed(int row, int col) const {
    for (const auto& trap : revealedTraps) {
        if (trap.first == row && trap.second == col) {
            return true;
        }
    }
    return false;
}
