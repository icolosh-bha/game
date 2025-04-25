#include "game.hpp"
#include <iostream>
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "textureManager.h"
#include "map.h"
#include "monster.h"

SDL_Renderer* Game::renderer = nullptr;

Game::Game() :
    isRunning(false),
    window(nullptr),
    playerTexture(nullptr),
    keyTexture(nullptr),
    goalTexture(nullptr),
    trapTexture(nullptr),
    gameMap(nullptr),
    monster(nullptr),
    hasKey(false),
    monsterActive(false)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "SDL initialized successfully" << std::endl;

        // Get the current display mode
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        // Create window with fullscreen flag
        window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            displayMode.w, displayMode.h, SDL_WINDOW_FULLSCREEN_DESKTOP);

        if (window) {
            std::cout << "Window created successfully" << std::endl;
        } else {
            std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "Renderer created successfully" << std::endl;
        } else {
            std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        }

        isRunning = true;
    } else {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }

    // Initialize game objects
    gameMap = new Map();
    gameMap->Loadmap(); // Thêm dòng này để chắc chắn map được load
    // Load textures with debug messages
    playerTexture = TextureManager::LoadTexture("player.png");
    if (!playerTexture) {
        std::cout << "Failed to load player.png: " << SDL_GetError() << std::endl;
    }

    keyTexture = TextureManager::LoadTexture("key.png");
    if (!keyTexture) {
        std::cout << "Failed to load key.png: " << SDL_GetError() << std::endl;
    }

    goalTexture = TextureManager::LoadTexture("goal.png");
    if (!goalTexture) {
        std::cout << "Failed to load goal.png: " << SDL_GetError() << std::endl;
    }

    trapTexture = TextureManager::LoadTexture("trap.png");
    if (!trapTexture) {
        std::cout << "Failed to load trap.png: " << SDL_GetError() << std::endl;
    }

    if (!playerTexture) {
        std::cout << "NO load player.png: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Set up player position
    playerRect.w = TILE_SIZE;
    playerRect.h = TILE_SIZE;

    bool foundWall = false;
    int startRow = 0, startCol = 0;

    for (int row = 0; row < 20 && !foundWall; row++) {
        for (int col = 0; col < 25 && !foundWall; col++) {
            if (gameMap->getTileAt(row, col) == 0) {
                startRow = row;
                startCol = col;
                foundWall = true;
            }
        }
    }

    playerRect.x = startCol * 32 + 4;
    playerRect.y = startRow * 32 + 4;

    monster = new Monster(gameMap);
}

Game::~Game() {
    delete gameMap;
    clean();
    SDL_DestroyTexture(keyTexture);
    SDL_DestroyTexture(goalTexture);
    SDL_DestroyTexture(trapTexture);
    SDL_DestroyTexture(wallTexture);
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN) {
            // Thoát game khi nhấn ESC
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
                return;
            }

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

                // Cho phép di chuyển trên wall (type 0), path (type 1), trap (type 3), key (type 5)
                // và goal (type 4) nếu đã có key
                if (tile == 0 || tile == 1 || tile == 3 || tile == 5 || (tile == 4 && hasKey)) {
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

    // Nếu player dẫm lên trap thì lộ trap
    if (currentTile == 3) {
     //   trapRevealed = true;
        showMessage("YOU DIED!");
        isRunning = false;
    }
    // Kích hoạt quái vật ngay từ đầu
    monsterActive = true;
    if (monster == nullptr) {
        std::cout << "ERROR: Monster is null!" << std::endl;
    } else {
       // std::cout << "Monster object exists at position: " << monster->getRect()->x << ", " << monster->getRect()->y << std::endl;
    }

    // If on a trap tile, reveal it and end the game
    if (currentTile == 3) {
        // Keep the trap visible
        showMessage("YOU DIED!");
        isRunning = false;
    }
    // If on a key tile
    else if (currentTile == 5) {
        hasKey = true;
        gameMap->UpdateTile(tileY, tileX, 1); // Change key tile to path
        showMessage("YOU GOT THE KEY!");
        if (monster) {
        monster->setKeyStatus(true);  // <- Dòng quan trọng!
    }
    }
    // If on goal tile
    else if (currentTile == 4) {
        if (hasKey) {
            showMessage("CONGARLUATION! THIS GAME IS YOURS");
            isRunning = false;
        } else {
            showMessage("YOU HAVE TO TAKE THE KEY");
        }
    }

    // Cập nhật vị trí quái vật
    if (monsterActive && monster != nullptr) {
        monster->update(playerRect.x, playerRect.y);
        SDL_Rect monsterRect = *monster->getRect();
        if (SDL_HasIntersection(&playerRect, &monsterRect)) {

            showMessage("MONSTER CAUGHT YOU!");
            isRunning = false;
            return;
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Vẽ map bằng hàm Drawmap
    gameMap->Drawmap();

    // Vẽ player
    SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);

    // Render monster if active
    if (monsterActive && monster != nullptr) {
        monster->render();
    }

    // Vẽ key và goal như cũ (không thay đổi)
    int keyW = 0, keyH = 0;
    SDL_QueryTexture(keyTexture, nullptr, nullptr, &keyW, &keyH);

    SDL_Rect src = { 0, 0, keyW, keyH };

    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 25; col++) {
            if (gameMap->getTileAt(row, col) == 5) {
                SDL_Rect dst = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, keyTexture, &src, &dst);
            } else if (gameMap->getTileAt(row, col) == 4) {
                SDL_Rect dst = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                int goalW = 0, goalH = 0;
                SDL_QueryTexture(goalTexture, nullptr, nullptr, &goalW, &goalH);
                SDL_Rect goalSrc = { 0, 0, goalW, goalH };
                SDL_RenderCopy(renderer, goalTexture, &goalSrc, &dst);
            } else if (gameMap->getTileAt(row, col) == 3) {
                SDL_Rect dst = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                int trapW = 0, trapH = 0;
                SDL_QueryTexture(trapTexture, nullptr, nullptr, &trapW, &trapH);
                SDL_Rect trapSrc = { 0, 0, trapW, trapH };
                SDL_RenderCopy(renderer, trapTexture, &trapSrc, &dst);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game Cleaned!" << std::endl;
}

void Game::showMessage(const char* message) {
    std::cout << "Message displayed: " << message << std::endl;

    // Vẽ một hình chữ nhật màu đen để làm nền cho tin nhắn
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);
    SDL_Rect messageBox = {
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    };
    SDL_RenderFillRect(renderer, &messageBox);

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return;
    }

    // Mở font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 36);
    if (!font) {
        std::cout << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return;
    }

    // Tạo surface chứa text
    SDL_Color textColor = {255, 255, 255, 255}; // Màu trắng
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, textColor);
    if (!textSurface) {
        std::cout << "TTF_RenderText_Solid failed: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        return;
    }

    // Tạo texture từ surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cout << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        TTF_Quit();
        return;
    }

    // Tính toán vị trí để căn giữa text
    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = (SCREEN_WIDTH - textRect.w) / 2;
    textRect.y = (SCREEN_HEIGHT - textRect.h) / 2;

    // Vẽ text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Giải phóng bộ nhớ
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_RenderPresent(renderer);
    SDL_Delay(3000); // Hiển thị trong 3 giây
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    // Sửa lỗi: phải gọi hàm với dấu ngoặc
    // gameMap->Loadmap;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems Initialised!..." << std::endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            std::cout << "Window created!" << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "Renderer created!" << std::endl;
        }

        isRunning = true;
    } else {
        isRunning = false;
    }

    // Load textures
    playerTexture = TextureManager::LoadTexture("player.png");
    keyTexture = TextureManager::LoadTexture("key.png");
    goalTexture = TextureManager::LoadTexture("goal.png");
    trapTexture = TextureManager::LoadTexture("trap.png");

    // Initialize player position
    playerRect.x = TILE_SIZE;
    playerRect.y = TILE_SIZE;
    playerRect.w = TILE_SIZE;
    playerRect.h = TILE_SIZE;

    // Initialize map
    if (!gameMap) {
        gameMap = new Map();
    }
    gameMap->Loadmap(); // Sửa lỗi: thêm dấu ngoặc để gọi hàm

    wallTexture = TextureManager::LoadTexture("wall.png");
    if (!wallTexture) {
        std::cout << "Không load được wall.png: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Đã load thành công wall.png!" << std::endl;
    }
    monster = new Monster(gameMap);
}
