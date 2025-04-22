#include "monster.h"
#include "textureManager.h"
#include "map.h"
#include "game.hpp"
#include <SDL.h>
#include <iostream>
#include <cmath>

Monster::Monster(Map* map) : gameMap(map) {
    texture = TextureManager::LoadTexture("monster.png");  // Use trap.png as temporary texture
    if (!texture) {
        std::cout << "Failed to load trap.png: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Monster texture loaded successfully" << std::endl;
    }

    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    rect.x = 14 * TILE_SIZE;  // Start at bottom right, but not too close to the edge
    rect.y = 14 * TILE_SIZE;

    std::cout << "Monster initialized at position: " << rect.x << ", " << rect.y << std::endl;
}

void Monster::update(int playerX, int playerY) {
    moveTowardsPlayer(playerX, playerY);
}

SDL_Rect* Monster::getRect() {
    return &rect;
}

void Monster::render() {
    if (!texture) {
        std::cout << "ERROR: Monster texture is null!" << std::endl;
        return;
    }

    // Vẽ một hình tròn màu đỏ xung quanh quái vật


    // Vẽ quái vật
    if (SDL_RenderCopy(Game::renderer, texture, NULL, &rect) != 0) {
        std::cout << "Failed to render monster: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Monster rendered successfully at: " << rect.x << ", " << rect.y << std::endl;
    }

}

void Monster::moveTowardsPlayer(int playerX, int playerY) {
    // Calculate direction to player
    float dx = playerX - rect.x;
    float dy = playerY - rect.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }

    // Try to move in X direction
    int newX = rect.x + (dx > 0 ? 32 : -32);
    int newTileX = (newX + 12) / 32;
    int currentTileY = (rect.y + 12) / 32;

    if (newTileX >= 0 && newTileX < 25 &&
        (gameMap->getTileAt(currentTileY, newTileX) == 0 ||
         gameMap->getTileAt(currentTileY, newTileX) == 1)) {
        rect.x = newX;
        return;
    }

    // Try to move in Y direction
    int newY = rect.y + (dy > 0 ? 32 : -32);
    int newTileY = (newY + 12) / 32;
    int currentTileX = (rect.x + 12) / 32;

    if (newTileY >= 0 && newTileY < 20 &&
        (gameMap->getTileAt(newTileY, currentTileX) == 0 ||
         gameMap->getTileAt(newTileY, currentTileX) == 1)) {
        rect.y = newY;
    }
}
