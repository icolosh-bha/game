#include "map.h"
#include "textureManager.h"
#include <iostream>

int lvl1[21][25] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,4,2},
    {2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2},
    {2,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,2},
    {2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2},
    {2,0,0,0,2,0,0,0,2,0,3,0,2,0,0,0,2,0,0,0,0,0,2,0,2},
    {2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,2,2,2,2,2,2,0,2},
    {2,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
    {2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,2,0,0,0,2,0,5,0,2,0,0,0,2,0,3,0,0,0,0,0,2},
    {2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,2,2,0,2,2,2,2,2,0,2},
    {2,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,2},
    {2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2},
    {2,0,0,0,2,0,0,0,2,0,0,0,2,0,3,0,2,0,0,0,0,0,2,0,2},
    {2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,2,2,2,2,2,2,0,2},
    {2,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
    {2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,2,0,0,0,2,0,3,0,2,0,0,0,2,0,0,0,0,0,0,0,2},
    {2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,2,2,0,2,2,2,2,2,0,2},
    {2,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};

Map::Map() {
    wall = TextureManager::LoadTexture("wall.png");
    dirt = TextureManager::LoadTexture("dirt.png");
    cloudy = TextureManager::LoadTexture("cloudy.png");
    trap = TextureManager::LoadTexture("trap.png");
    key = TextureManager::LoadTexture("key.png");
    goal = TextureManager::LoadTexture("goal.png");

    // Debug messages
    std::cout << "Loading textures:" << std::endl;
    std::cout << "wall: " << (wall ? "success" : "failed") << std::endl;
    std::cout << "dirt: " << (dirt ? "success" : "failed") << std::endl;
    std::cout << "cloudy: " << (cloudy ? "success" : "failed") << std::endl;
    std::cout << "trap: " << (trap ? "success" : "failed") << std::endl;
    std::cout << "key: " << (key ? "success" : "failed") << std::endl;
    std::cout << "goal: " << (goal ? "success" : "failed") << std::endl;

    src.x = src.y = 0;
    src.w = dest.w = 32;
    src.h = dest.h = 32;
    dest.x = dest.y = 0;

    Loadmap();
}

Map::~Map() {
    SDL_DestroyTexture(wall);
    SDL_DestroyTexture(dirt);
    SDL_DestroyTexture(cloudy);
    SDL_DestroyTexture(trap);
    SDL_DestroyTexture(key);
    SDL_DestroyTexture(goal);
}

void Map::RevealTrap(int row, int col) {
    if (row >= 0 && row < 20 && col >= 0 && col < 25) {
        discoveredTraps[row][col] = true;
    }
}

void Map::Loadmap() {
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 25; col++) {
            map[row][col] = lvl1[row][col];
        }
    }
}

int Map::getTileAt(int row, int col) {
    if (row < 0 || row >= 20 || col < 0 || col >= 25) return -1;
    return map[row][col];
}

void Map::UpdateTile(int row, int col, int newValue) {
    if (row >= 0 && row < 20 && col >= 0 && col < 25) {
        map[row][col] = newValue;
    }
}

void Map::Drawmap() {
    // Set background color to light gray
    SDL_SetRenderDrawColor(TextureManager::renderer, 200, 200, 200, 255);
    SDL_RenderClear(TextureManager::renderer);

    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 25; col++) {
            int type = map[row][col];
            dest.x = col * 32;
            dest.y = row * 32;

            // Draw a colored rectangle for each tile type
            switch (type) {
                case 0: // wall
                    SDL_SetRenderDrawColor(TextureManager::renderer, 100, 100, 100, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (wall) {
                        SDL_RenderCopy(TextureManager::renderer, wall, &src, &dest);
                    }
                    break;
                case 1: // dirt
                    SDL_SetRenderDrawColor(TextureManager::renderer, 150, 75, 0, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (dirt) {
                        SDL_RenderCopy(TextureManager::renderer, dirt, &src, &dest);
                    }
                    break;
                case 2: // cloudy
                    SDL_SetRenderDrawColor(TextureManager::renderer, 200, 200, 255, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (cloudy) {
                        SDL_RenderCopy(TextureManager::renderer, cloudy, &src, &dest);
                    }
                    break;
                case 3: // trap - draw as wall unless discovered
                    SDL_SetRenderDrawColor(TextureManager::renderer, 100, 100, 100, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (discoveredTraps[row][col]) {
                        if (trap) {
                            SDL_RenderCopy(TextureManager::renderer, trap, &src, &dest);
                        }
                    } else {
                        if (wall) {
                            SDL_RenderCopy(TextureManager::renderer, wall, &src, &dest);
                        }
                    }
                    break;
                case 4: // goal
                    SDL_SetRenderDrawColor(TextureManager::renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (goal) {
                        SDL_RenderCopy(TextureManager::renderer, goal, &src, &dest);
                    }
                    break;
                case 5: // key
                    SDL_SetRenderDrawColor(TextureManager::renderer, 255, 255, 0, 255);
                    SDL_RenderFillRect(TextureManager::renderer, &dest);
                    if (key) {
                        SDL_RenderCopy(TextureManager::renderer, key, &src, &dest);
                    }
                    break;
                default: break;
            }
        }
    }
}
