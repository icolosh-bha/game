#include "game.hpp"
#include <iostream>
#include <SDL.h>
#include <C:\Users\hangu\Downloads\SDL2_image-devel-2.8.4-mingw\SDL2_image-2.8.4\x86_64-w64-mingw32\include\SDL2\SDL_image.h>
#include "textureManager.h"
#include "map.h"

SDL_Renderer* Game::renderer = nullptr;

bool fileExists(const char* path) {
    SDL_RWops* file = SDL_RWFromFile(path, "r");
    if (file) { SDL_RWclose(file); return true; }
    return false;
}

Game::Game() : isRunning(false), window(nullptr), playerTexture(nullptr), gameMap(nullptr), hasKey(false) {}

Game::~Game() {
    delete gameMap;
    clean();
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Khởi tạo SDL_image với PNG support
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Tạo window
    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Tạo renderer với hardware acceleration và vsync
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Set renderer draw color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Thiết lập renderer cho TextureManager
    TextureManager::renderer = renderer;

    // Load game map và player
    gameMap = new Map();
    playerTexture = TextureManager::LoadTexture("player.png");
    if (!playerTexture) {
        std::cout << "Không load được player.png: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    int w, h;
    SDL_QueryTexture(playerTexture, nullptr, nullptr, &w, &h);
    playerRect.w = 24;  // Thu nhỏ kích thước nhân vật xuống 24x24 pixels
    playerRect.h = 24;
    
    // Tìm vị trí wall ngẫu nhiên để đặt nhân vật
    bool foundWall = false;
    int startRow = 0, startCol = 0;
    
    // Tìm vị trí wall đầu tiên
    for (int row = 0; row < 20 && !foundWall; row++) {
        for (int col = 0; col < 25 && !foundWall; col++) {
            if (gameMap->getTileAt(row, col) == 0) {
                startRow = row;
                startCol = col;
                foundWall = true;
            }
        }
    }
    
    // Đặt nhân vật ở vị trí wall
    playerRect.x = startCol * 32 + 4;  // Căn giữa nhân vật trong ô
    playerRect.y = startRow * 32 + 4;

    isRunning = true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) isRunning = false;
        if (event.type == SDL_KEYDOWN) {
            int newX = playerRect.x;
            int newY = playerRect.y;
            
            switch (event.key.keysym.sym) {
                case SDLK_RIGHT: newX += 32; break;
                case SDLK_LEFT:  newX -= 32; break;
                case SDLK_UP:    newY -= 32; break;
                case SDLK_DOWN:  newY += 32; break;
            }
            
            // Kiểm tra va chạm với các ô
            int newRow = newY / 32;
            int newCol = newX / 32;
            
            if (newRow >= 0 && newRow < 20 && newCol >= 0 && newCol < 25) {
                int tile = gameMap->getTileAt(newRow, newCol);
                
                // Cho phép di chuyển trên wall (type 0), trap (type 3), key (type 5)
                // và goal (type 4) nếu đã có key
                if (tile == 0 || tile == 3 || tile == 5 || (tile == 4 && hasKey)) {
                    playerRect.x = newCol * 32 + 4;  // Căn giữa nhân vật trong ô
                    playerRect.y = newRow * 32 + 4;
                }
            }
        }
    }
}

void Game::update() {
    // Calculate current tile position
    int tileX = (playerRect.x + 12) / 32;
    int tileY = (playerRect.y + 12) / 32;
    
    // Get the current tile type
    int currentTile = gameMap->getTileAt(tileY, tileX);
    
    // If on a trap tile, reveal it and end the game
    if (currentTile == 3) {
        gameMap->RevealTrap(tileY, tileX);
        showMessage("BẠN ĐÃ CHẾT!");
        isRunning = false;
    }
    // If on a key tile
    else if (currentTile == 5) {
        hasKey = true;
        gameMap->UpdateTile(tileY, tileX, 1); // Change key tile to path
        showMessage("BẠN ĐÃ LẤY ĐƯỢC CHÌA KHÓA!");
    }
    // If on goal tile
    else if (currentTile == 4) {
        if (hasKey) {
            showMessage("BẠN ĐÃ THẮNG TRÒ CHƠI!");
            isRunning = false;
        } else {
            showMessage("BẠN CẦN LẤY CHÌA KHÓA TRƯỚC!");
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    gameMap->Drawmap();
    SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void Game::showMessage(const char* message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Trò chơi", message, window);
    SDL_Delay(1000);
}
