#include "monster.h"
#include "textureManager.h"
#include "map.h"
#include "game.hpp"
#include <SDL.h>
#include <iostream>
#include <cmath>
#include <queue>
#include <utility>
#include <vector>
#include <algorithm>
Monster::Monster(Map* map) : gameMap(map), isActive(false) {
    texture = TextureManager::LoadTexture("monster.png");
    if (!texture) {
        std::cout << "Failed to load monster.png: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Monster texture loaded successfully" << std::endl;
    }

    rect.w = 1.2  * TILE_SIZE;
    rect.h = 1.2  * TILE_SIZE;
    rect.x = 15 * TILE_SIZE;  // Start at bottom right, but not too close to the edge
    rect.y = 15 * TILE_SIZE;

    std::cout << "Monster initialized at position: " << rect.x << ", " << rect.y << std::endl;
}

void Monster::update(int playerX, int playerY) {
    if (!isActive) return;

    int startX = (rect.x + rect.w / 2) / TILE_SIZE;
    int startY = (rect.y + rect.h / 2) / TILE_SIZE;
    int goalX = (playerX + rect.w / 2) / TILE_SIZE;
    int goalY = (playerY + rect.h / 2) / TILE_SIZE;

    std::cout << "\n[Monster] Start: (" << startY << ", " << startX << "), Goal: (" << goalY << ", " << goalX << ")\n";

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    bool visited[20][25] = {};
    std::pair<int, int> parent[20][25];

    std::queue<std::pair<int, int>> q;
    q.push({startY, startX});
    visited[startY][startX] = true;
    parent[startY][startX] = {-1, -1};

    bool found = false;
    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        if (x == goalX && y == goalY) {
            found = true;
            break;
        }

        for (int dir = 0; dir < 4; ++dir) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx >= 0 && nx < 25 && ny >= 0 && ny < 20 && !visited[ny][nx]) {
                int tile = gameMap->getTileAt(ny, nx);
                if (tile == 0 || tile == 1 || tile == 3) {
                    visited[ny][nx] = true;
                    parent[ny][nx] = {y, x};
                    q.push({ny, nx});
                }
            }
        }
    }

    int nextX = startX, nextY = startY;
    if (found) {
        std::vector<std::pair<int, int>> path;
        int cx = goalX, cy = goalY;
        while (!(cx == startX && cy == startY)) {
            path.push_back({cy, cx});
            auto p = parent[cy][cx];
            cy = p.first;
            cx = p.second;
        }
        std::reverse(path.begin(), path.end());

        if (!path.empty()) {
            nextY = path[0].first;
            nextX = path[0].second;

            int currentY = (rect.y + rect.h / 2) / TILE_SIZE;
            int currentX = (rect.x + rect.w / 2) / TILE_SIZE;

            // Nếu đang ở bước đầu rồi, lấy bước tiếp theo nếu có
            if (currentY == nextY && currentX == nextX && path.size() > 1) {
                nextY = path[1].first;
                nextX = path[1].second;
            }
        }

        std::cout << "Next tile target: (" << nextY << ", " << nextX << ")\n";
    } else {
        std::cout << "No path found to player!\n";
    }

    int tileType = gameMap->getTileAt(startY, startX);
    if (tileType == 3) {
        std::cout << "[Monster] Stepped on tile 3 — dead!\n";
        isActive = false;
        return;
    }

    float targetX = nextX * TILE_SIZE + TILE_SIZE / 2 - rect.w / 2;
    float targetY = nextY * TILE_SIZE + TILE_SIZE / 2 - rect.h / 2;
    float dxMove = targetX - rect.x;
    float dyMove = targetY - rect.y;
    float dist = std::sqrt(dxMove * dxMove + dyMove * dyMove);
    const float SPEED = 1.0f;

    if (dist > 0.1f) {
        float moveX = (std::abs(dxMove) > SPEED) ? SPEED * (dxMove > 0 ? 1 : -1) : dxMove;
        float moveY = (std::abs(dyMove) > SPEED) ? SPEED * (dyMove > 0 ? 1 : -1) : dyMove;

        // Cho phép di chuyển cả hai trục nếu cần
        if (std::abs(dxMove) > 0.1f) rect.x += moveX;
        if (std::abs(dyMove) > 0.1f) rect.y += moveY;

        int tileX = (rect.x + rect.w / 2) / TILE_SIZE;
        int tileY = (rect.y + rect.h / 2) / TILE_SIZE;
        std::cout << "Monster moved to tile: (" << tileY << ", " << tileX
                  << "), type=" << gameMap->getTileAt(tileY, tileX) << "\n";
    }
}


SDL_Rect* Monster::getRect() {
    return &rect;
}

void Monster::render() {
    if (!texture) {
        std::cout << "ERROR: Monster texture is null!" << std::endl;
        return;
    }

    // Vẽ quái vật
    if (SDL_RenderCopy(Game::renderer, texture, NULL, &rect) != 0) {
        std::cout << "Failed to render monster: " << SDL_GetError() << std::endl;
    }
}

void Monster::moveTowardsPlayer(int playerX, int playerY) {
    float dx = playerX - rect.x;
    float dy = playerY - rect.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    const float SPEED = 0.05f;

    if (distance > 0) {
        dx /= distance;
        dy /= distance;

        float newX = rect.x + dx * SPEED;
        float newY = rect.y + dy * SPEED;

        // Kiểm tra và cập nhật từng trục độc lập
        int newTileX = (int)(newX + rect.w/2) / TILE_SIZE;
        int currentTileY = (int)(rect.y + rect.h/2) / TILE_SIZE;
        if (newTileX >= 0 && newTileX < 25 &&
            (gameMap->getTileAt(currentTileY, newTileX) == 0 ||
             gameMap->getTileAt(currentTileY, newTileX) == 1)) {
            rect.x = (int)newX;
        }

        int newTileY = (int)(newY + rect.h/2) / TILE_SIZE;
        int currentTileX = (int)(rect.x + rect.w/2) / TILE_SIZE;
        if (newTileY >= 0 && newTileY < 20 &&
            (gameMap->getTileAt(newTileY, currentTileX) == 0 ||
             gameMap->getTileAt(newTileY, currentTileX) == 1)) {
            rect.y = (int)newY;
        }
    }
}

void Monster::setKeyStatus(bool hasKey) {
    isActive = hasKey;
    if (isActive) {
        std::cout << "Monster activated! Now chasing player." << std::endl;
    }
}
#include "game.hpp"
