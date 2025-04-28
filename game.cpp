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
    wallTexture(nullptr),
    defaultFont(nullptr),
    gameMap(nullptr),
    monster(nullptr),
    hasKey(false),
    monsterActive(false),
    lives(3),
    initialPlayerX(0),
    initialPlayerY(0)
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

    initialPlayerX = playerRect.x;
    initialPlayerY = playerRect.y;

    monster = new Monster(gameMap);
}

Game::~Game() {
    if (defaultFont != nullptr) {
        TTF_CloseFont(defaultFont);
    }
    TTF_Quit();

    delete gameMap;
    clean();

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(keyTexture);
    SDL_DestroyTexture(goalTexture);
    SDL_DestroyTexture(trapTexture);
    SDL_DestroyTexture(wallTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Game Cleaned!" << std::endl;
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

    // Get current tile type
    int currentTile = gameMap->getTileAt(tileY, tileX);

    // Kích hoạt quái vật ngay từ đầu
    monsterActive = true;
    if (monster == nullptr) {
        std::cout << "ERROR: Monster is null!" << std::endl;
    }

    // Check current tile effects
    if (currentTile == 3) { // Trap tile
        lives--;

        // Tìm trap gần nhất chưa reveal
        int nearestTrapRow = -1, nearestTrapCol = -1;
        int minDist = INT_MAX;

        for (int row = 0; row < 20; row++) {
            for (int col = 0; col < 25; col++) {
                if (gameMap->getTileAt(row, col) == 3 && !gameMap->isTrapRevealed(row, col)) {
                    int dist = abs(row - tileY) + abs(col - tileX);
                    if (dist < minDist) {
                        minDist = dist;
                        nearestTrapRow = row;
                        nearestTrapCol = col;
                    }
                }
            }
        }

        // Reveal trap gần nhất nếu tìm thấy
        if (nearestTrapRow != -1 && nearestTrapCol != -1) {
            gameMap->revealTrap(nearestTrapRow, nearestTrapCol);
        }



        char message[100];
        sprintf(message, "YOU DIED!\nLives left: %d\nDo you want to continue? (Y/N)", lives);

        if (lives > 0) {
            if (showConfirmMessage(message)) {
                resetPlayerPosition();
              //  gameMap->setShowTraps(false); // Hide traps again
                return;
            } else {
                showMessage("GAME OVER!");
                isRunning = false;
                return;
            }
        } else {
            showMessage("GAME OVER! No lives left!");
            isRunning = false;
            return;
        }
    }
    // If on a key tile
    else if (currentTile == 5) {
        hasKey = true;
        gameMap->UpdateTile(tileY, tileX, 1); // Change key tile to path
        showMessage("YOU GOT THE KEY!");
        if (monster) {
            monster->setKeyStatus(true);
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
            lives--;

            char message[100];
            sprintf(message, "MONSTER CAUGHT YOU! Lives left: %d\nDo you want to continue? (Y/N)", lives);

            if (lives > 0) {
                if (showConfirmMessage(message)) {
                    resetPlayerPosition();
                    return;
                } else {
                    showMessage("GAME OVER!");
                    isRunning = false;
                    return;
                }
            } else {
                showMessage("GAME OVER! No lives left!");
                isRunning = false;
                return;
            }
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
                //SDL_Rect dst = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
               // int trapW = 0, trapH = 0;
               // SDL_QueryTexture(trapTexture, nullptr, nullptr, &trapW, &trapH);
                //SDL_Rect trapSrc = { 0, 0, trapW, trapH };
                //SDL_RenderCopy(renderer, trapTexture, &trapSrc, &dst);
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
    TTF_Quit();
    std::cout << "Game Cleaned!" << std::endl;
}

void Game::initFont() {
    // Khởi tạo thư viện SDL_ttf nếu chưa init
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cout << "[TTF] Init failed: " << TTF_GetError() << std::endl;
            return;
        }
    }

    // Load font vào defaultFont
    defaultFont = TTF_OpenFont("arial.ttf", 36);
    if (!defaultFont) {
        std::cout << "[TTF] OpenFont failed: " << TTF_GetError() << std::endl;
    } else {
        std::cout << "[TTF] Font loaded successfully!" << std::endl;
    }
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    // Tạo window
    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (window == nullptr) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Tạo renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    isRunning = true;

    // Load textures
    playerTexture = TextureManager::LoadTexture("player.png");
    keyTexture = TextureManager::LoadTexture("key.png");
    goalTexture = TextureManager::LoadTexture("goal.png");
    trapTexture = TextureManager::LoadTexture("trap.png");
    wallTexture = TextureManager::LoadTexture("wall.png");

    // Set player rect
    playerRect.w = TILE_SIZE - 8;
    playerRect.h = TILE_SIZE - 8;

    // Create and load map
    gameMap = new Map();
    gameMap->Loadmap();

    // Khởi tạo font nếu chưa được khởi tạo
    if (defaultFont == nullptr) {
        initFont();
    }
    monster = new Monster(gameMap);
    if (monster != nullptr) {
    SDL_Rect* mRect = monster->getRect();
    initialMonsterX = mRect->x;
    initialMonsterY = mRect->y;
}
}

void Game::resetPlayerPosition() {
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

    // Reset monster position if needed
    if (monster != nullptr) {
        monster->getRect()->x = initialMonsterX;
        monster->getRect()->y = initialMonsterY;
    }
}

bool Game::showConfirmMessage(const char* message) {
    bool quit = false;
    bool result = false;

    // Lưu trữ renderer hiện tại
    //SDL_SetRenderTarget(renderer, currentScreen);
    SDL_RenderCopy(renderer, NULL, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);

    while (!quit) {
        // Vẽ màn hình game hiện tại
     //   SDL_RenderCopy(renderer, currentScreen, NULL, NULL);
       SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230); // nền tối
        SDL_RenderClear(renderer);
        SDL_Rect messageBox = {
            200,  // SCREEN_WIDTH/4
            213,  // SCREEN_HEIGHT/3
            400,  // SCREEN_WIDTH/2
            213   // SCREEN_HEIGHT/3
        };
        SDL_RenderFillRect(renderer, &messageBox);

        // Hiển thị thông báo chính
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(defaultFont, message, textColor, messageBox.w - 40);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    messageBox.x + 20,
                    messageBox.y + 20,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Hiển thị hướng dẫn
        const char* hint = "";
       SDL_Surface* hintSurface = TTF_RenderText_Solid(defaultFont, hint, textColor);
        if (hintSurface) {
            SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(renderer, hintSurface);
            if (hintTexture) {
                SDL_Rect hintRect = {
                    messageBox.x + (messageBox.w - hintSurface->w) / 2,
                    messageBox.y + messageBox.h - 40,
                    hintSurface->w,
                    hintSurface->h
                };
                SDL_RenderCopy(renderer, hintTexture, NULL, &hintRect);
                SDL_DestroyTexture(hintTexture);
            }
            SDL_FreeSurface(hintSurface);
        }

        SDL_RenderPresent(renderer);

        // Xử lý input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_y:
                    case SDLK_RETURN:
                        result = true;
                        quit = true;
                        break;
                    case SDLK_n:
                    case SDLK_ESCAPE:
                        result = false;
                        quit = true;
                        break;
                }
            }
        }
        SDL_Delay(4);
    }

   // SDL_DestroyTexture(currentScreen);
    return result;
}

void Game::removeTrapNearPosition(int x, int y) {
    int playerTileX = x / TILE_SIZE;
    int playerTileY = y / TILE_SIZE;
    int minDistance = INT_MAX;
    int trapRow = -1, trapCol = -1;

    // Search for the nearest trap
    for (int row = 0; row < 21; row++) {
        for (int col = 0; col < 25; col++) {
            if (gameMap->getTileAt(row, col) == 3) {  // If it's a trap
                int distance = abs(row - playerTileY) + abs(col - playerTileX);
                if (distance < minDistance) {
                    minDistance = distance;
                    trapRow = row;
                    trapCol = col;
                }
            }
        }
    }

    // If we found a trap, remove it
    if (trapRow != -1 && trapCol != -1) {
        gameMap->UpdateTile(trapRow, trapCol, 1);  // Change trap to path
    }
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

    // Mở font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 36);
    if (!font) {
        std::cout << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
       // TTF_Quit();
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
   // TTF_Quit();

    SDL_RenderPresent(renderer);
    SDL_Delay(3000); // Hiển thị trong 3 giây
}
