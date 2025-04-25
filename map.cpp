#include "map.h"
#include "textureManager.h"
#include "game.hpp"
#include <iostream>

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
    {2,0,0,0,0,0,0,2,0,0,0,3,0,0,2,0,0,0,0,0,0,0,0,4,2},
    {2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,0,2},
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

    trap = nullptr;  // Don't load trap texture initially

    src.x = src.y = 0;
    src.w = dest.w = 32;
    src.h = dest.h = 32;
    dest.x = dest.y = 0;

    Loadmap();
    showTraps = false;
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
    // Set background color to light gray
    SDL_SetRenderDrawColor(Game::renderer, 200, 200, 200, 255);
    SDL_RenderClear(Game::renderer);

    for (int row = 0; row <= 20; row++) {
        for (int col = 0; col <= 25; col++) {
            int type = map[row][col];
            dest.x = col * 32;
            dest.y = row * 32;

            switch (type) {
                case 0: // wall
                    SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (wall) {
                        SDL_RenderCopy(Game::renderer, wall, &src, &dest);
                    }
                    break;
                case 1: // dirt
                    SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (dirt) {
                        SDL_RenderCopy(Game::renderer, dirt, &src, &dest);
                    }
                    break;
                case 2: // cloudy
                    SDL_SetRenderDrawColor(Game::renderer, 175, 238, 238, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (cloudy) {
                        SDL_RenderCopy(Game::renderer, cloudy, &src, &dest);
                    }
                    break;
                case 3: // trap
                    SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (showTraps && trap) {
                        SDL_RenderCopy(Game::renderer, trap, &src, &dest);
                    } else if (wall) {
                        SDL_RenderCopy(Game::renderer, wall, &src, &dest);
                    }
                    break;
                case 4: // goal
                    SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (goal) {
                       // std::cout << "Rendering goal at position: (" << dest.x << ", " << dest.y << ")" << std::endl;
                        SDL_RenderCopy(Game::renderer, goal, &src, &dest);
                    } else {
                        std::cout << "Goal texture is null!" << std::endl;
                    }
                    break;
                case 5: // key
                    SDL_SetRenderDrawColor(Game::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(Game::renderer, &dest);
                    if (key) {
                        SDL_RenderCopy(Game::renderer, key, &src, &dest);
                    }
                    break;
                default: break;
            }
        }
    }
}
