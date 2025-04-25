
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

    // Lấy vị trí tile hiện tại của quái vật và player
    int startX = (rect.x + rect.w / 2) / TILE_SIZE;
    int startY = (rect.y + rect.h / 2) / TILE_SIZE;
    int goalX = (playerX + rect.w / 2) / TILE_SIZE;
    int goalY = (playerY + rect.h / 2) / TILE_SIZE;

    // BFS để tìm đường đi ngắn nhất
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    bool visited[20][25] = {};
    std::pair<int, int> parent[20][25];

    std::queue<std::pair<int, int> > q;
    q.push(std::make_pair(startY, startX));
    visited[startY][startX] = true;
    parent[startY][startX] = std::make_pair(-1, -1);

    bool found = false;
    while (!q.empty()) {
        std::pair<int, int> front = q.front(); q.pop();
        int y = front.first;
        int x = front.second;
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
                    parent[ny][nx] = std::make_pair(y, x);
                    q.push(std::make_pair(ny, nx));
                }
            }
        }
    }

    // Nếu tìm được đường đi, truy vết ngược để lấy bước tiếp theo
    int nextX = startX, nextY = startY;
    if (found) {
        int cx = goalX, cy = goalY;
        std::vector<std::pair<int, int> > path;
        while (!(cx == startX && cy == startY)) {
            path.push_back(std::make_pair(cy, cx));
            std::pair<int, int> p = parent[cy][cx];
            cy = p.first;
            cx = p.second;
        }
        // Lấy bước tiếp theo trên đường đi (nếu có)
        if (!path.empty()) {
            nextY = path.back().first;
            nextX = path.back().second;
        }
    }

    // Nếu đã đến tile 3 thì dừng lại
    int tileType = gameMap->getTileAt(startY, startX);
    if (tileType == 3) {
        std::cout << "Monster died!" << std::endl;
        isActive = false;
        return;
    }

    // Di chuyển mượt về tile tiếp theo (nếu khác tile hiện tại)
    float targetX = nextX * TILE_SIZE + TILE_SIZE / 2 - rect.w / 2;
    float targetY = nextY * TILE_SIZE + TILE_SIZE / 2 - rect.h / 2;
    float dxMove = targetX - rect.x;
    float dyMove = targetY - rect.y;
    float dist = std::sqrt(dxMove * dxMove + dyMove * dyMove);
    const float SPEED = 1.0f; // Có thể chỉnh cho mượt hơn

    if (dist > 0.1f) {
        float moveX = (std::abs(dxMove) > SPEED) ? SPEED * (dxMove > 0 ? 1 : -1) : dxMove;
        float moveY = (std::abs(dyMove) > SPEED) ? SPEED * (dyMove > 0 ? 1 : -1) : dyMove;
        // Ưu tiên di chuyển từng trục một để không bị đi chéo
        if (std::abs(dxMove) > 0.1f) {
            rect.x += moveX;
        } else if (std::abs(dyMove) > 0.1f) {
            rect.y += moveY;
        }
    }

    // In vị trí hiện tại của quái vật
//std::cout << "Monster position: (" << rect.x << ", " << rect.y << ")" << std::endl;
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

    const float SPEED = 0.5f;

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
