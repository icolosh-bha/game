#include "game.hpp"
#include <iostream>
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "textureManager.h"
#include "map.h"
#include "monster.h"
#include <C:\Users\hangu\Downloads\SDL2_mixer-devel-2.6.3-mingw\SDL2_mixer-2.6.3\x86_64-w64-mingw32\include\SDL2\SDL_mixer.h>
#include <fstream>
#include <string>
#include "soundManager.h"
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
    initialPlayerY(0),
    hasChosenDeathOption(false),
    hasFreezeAbility(false),
    isMonsterFrozen(false),
    freezeStartTime(0),
    freezeDuration(10000) // 10 seconds in milliseconds
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
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;

                case SDLK_RIGHT:
                    playerRect.x += 32;
                    break;
                case SDLK_LEFT:
                    playerRect.x -= 32;
                    break;
                case SDLK_UP:
                    playerRect.y -= 32;
                    break;
                case SDLK_DOWN:
                    playerRect.y += 32;
                    break;
                case SDLK_f:
                    if (hasFreezeAbility && !isMonsterFrozen) {
                        freezeMonster();
                    }
                    break;
            }

            // Sau khi tính new vị trí, cần kiểm tra hợp lệ
            int newRow = playerRect.y / 32;
            int newCol = playerRect.x / 32;

            if (newRow >= 0 && newRow < 20 && newCol >= 0 && newCol < 25) {
                int tile = gameMap->getTileAt(newRow, newCol);
                if (!(tile == 0 || tile == 1 || tile == 3 || tile == 5 || (tile == 4 && hasKey))) {
                    // Không hợp lệ → revert lại
                    playerRect.x -= (event.key.keysym.sym == SDLK_RIGHT) * 32;
                    playerRect.x += (event.key.keysym.sym == SDLK_LEFT) * 32;
                    playerRect.y += (event.key.keysym.sym == SDLK_UP) * 32;
                    playerRect.y -= (event.key.keysym.sym == SDLK_DOWN) * 32;
                }
            }
            break;
    }
}


void Game::update() {
    // Cập nhật trạng thái freeze quái vật
    updateMonsterFreezeState();

    // Vị trí tile hiện tại của player
    int tileX = (playerRect.x + 12) / 32;
    int tileY = (playerRect.y + 12) / 32;
    int currentTile = gameMap->getTileAt(tileY, tileX);

    // Kích hoạt quái vật
    monsterActive = true;
    if (monster == nullptr) {
        std::cout << "ERROR: Monster is null!" << std::endl;
    }

    // ======================= TRAP TILE =======================
    if (currentTile == 3) {
        lives--;

        // Reveal trap gần nhất chưa reveal
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

        if (nearestTrapRow != -1 && nearestTrapCol != -1) {
            gameMap->revealTrap(nearestTrapRow, nearestTrapCol);
        }

        char message[100];
        sprintf(message, "YOU DIED!\nLives left: %d\nDo you want to continue? (Y/N)", lives);

        if (lives > 0) {
            if (showConfirmMessage(message)) {
                      showDeathOptionsPrompt();
                SoundManager::get().playMusic("assets/sounds/death.wav");
                resetPlayerPosition();
                return;
            } else {
                SoundManager::get().playMusic("assets/sounds/death.wav");
                showMessage("GAME OVER!");
                isRunning = false;
                return;
            }
        } else {
            SoundManager::get().playMusic("assets/sounds/death.wav");
            showMessage("GAME OVER! No lives left!");
            isRunning = false;
            return;
        }
    }

    // ======================= KEY TILE =======================
    else if (currentTile == 5) {
        hasKey = true;
        gameMap->UpdateTile(tileY, tileX, 1); // Đổi tile về đường đi
        showMessage("YOU GOT THE KEY!");
        if (monster) {
            monster->setKeyStatus(true);
        }
    }

    // ======================= GOAL TILE =======================
    else if (currentTile == 4) {
        if (hasKey) {
            SoundManager::get().playMusic("assets/sounds/victory.wav");
            showMessage("CONGARLUATION! THIS GAME IS YOURS");
            isRunning = false;
        } else {
            showMessage("YOU HAVE TO TAKE THE KEY");
        }
    }

    // ======================= MONSTER COLLISION =======================
    if (monsterActive && monster != nullptr && !isMonsterFrozen) {
        monster->update(playerRect.x, playerRect.y);

        SDL_Rect monsterRect = *monster->getRect();
        if (SDL_HasIntersection(&playerRect, &monsterRect)) {
            lives--;

            char message[100];
            sprintf(message, "MONSTER CAUGHT YOU!\nLives left: %d\nDo you want to continue? (Y/N)", lives);

            if (lives > 0) {
                if (showConfirmMessage(message)) {
                       showDeathOptionsPrompt();
                    SoundManager::get().playMusic("assets/sounds/death.wav");
                    resetPlayerPosition();
                    return;
                } else {
                    SoundManager::get().playMusic("assets/sounds/death.wav");
                    showMessage("GAME OVER!");
                    isRunning = false;
                    return;
                }
            } else {
                SoundManager::get().playMusic("assets/sounds/death.wav");
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

/**
 * Save game state to a text file
 * @param filename The file to save to
 */
void Game::saveGameState(const std::string& filename) {
    std::ofstream saveFile(filename);
    if (!saveFile.is_open()) {
        std::cout << "Failed to open save file: " << filename << std::endl;
        return;
    }

    // Save player position
    saveFile << playerRect.x << " " << playerRect.y << std::endl;

    // Save lives
    saveFile << lives << std::endl;

    // Save key status
    saveFile << (hasKey ? "1" : "0") << std::endl;

    saveFile.close();
    std::cout << "Game state saved to " << filename << std::endl;
}

/**
 * Load game state from a text file
 * @param filename The file to load from
 * @return true if successful, false otherwise
 */
bool Game::loadGameState(const std::string& filename) {
    std::ifstream saveFile(filename);
    if (!saveFile.is_open()) {
        std::cout << "Failed to open save file: " << filename << std::endl;
        return false;
    }

    // Load player position
    saveFile >> playerRect.x >> playerRect.y;

    // Load lives
    saveFile >> lives;

    // Load key status
    std::string keyStatus;
    saveFile >> keyStatus;
    hasKey = (keyStatus == "1");

    saveFile.close();
    std::cout << "Game state loaded from " << filename << std::endl;
    return true;
}

/**
 * Check if a save file exists
 * @param filename The file to check
 * @return true if the file exists, false otherwise
 */
bool Game::hasSaveFile(const std::string& filename) const {
    std::ifstream saveFile(filename);
    return saveFile.good();
}

bool Game::showDeathOptionsPrompt() {
    if (hasChosenDeathOption) {
        return false; // Already chosen an option before
    }

    bool quit = false;
    int choice = 0; // 0 = no choice, 1 = remove trap, 2 = freeze ability

    // Save current screen
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_Texture* currentScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetRenderTarget(renderer, currentScreen);
    SDL_RenderCopy(renderer, NULL, NULL, NULL);
    SDL_SetRenderTarget(renderer, NULL);

    // Create message box
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);
    SDL_Rect messageBox = {
        200,  // SCREEN_WIDTH/4
        150,  // Adjusted position
        400,  // SCREEN_WIDTH/2
        300   // Adjusted height
    };

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        return false;
    }

    while (!quit) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, currentScreen, NULL, NULL);

        SDL_RenderFillRect(renderer, &messageBox);

        // Render title
        SDL_Color textColor = {255, 255, 255, 255};
        const char* title = "Choose an ability:";
        SDL_Surface* titleSurface = TTF_RenderText_Solid(font, title, textColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {
                    messageBox.x + (messageBox.w - titleSurface->w) / 2,
                    messageBox.y + 20,
                    titleSurface->w,
                    titleSurface->h
                };
                SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }

        // Render option 1
        SDL_Color option1Color = (choice == 1) ? SDL_Color{255, 255, 0, 255} : textColor;
        const char* option1 = "1. Remove the nearest trap";
        SDL_Surface* option1Surface = TTF_RenderText_Solid(font, option1, option1Color);
        if (option1Surface) {
            SDL_Texture* option1Texture = SDL_CreateTextureFromSurface(renderer, option1Surface);
            if (option1Texture) {
                SDL_Rect option1Rect = {
                    messageBox.x + 30,
                    messageBox.y + 80,
                    option1Surface->w,
                    option1Surface->h
                };
                SDL_RenderCopy(renderer, option1Texture, NULL, &option1Rect);
                SDL_DestroyTexture(option1Texture);
            }
            SDL_FreeSurface(option1Surface);
        }

        // Render option 2
        SDL_Color option2Color = (choice == 2) ? SDL_Color{255, 255, 0, 255} : textColor;
        const char* option2 = "2. Gain ability to freeze monster (press F)";
        SDL_Surface* option2Surface = TTF_RenderText_Solid(font, option2, option2Color);
        if (option2Surface) {
            SDL_Texture* option2Texture = SDL_CreateTextureFromSurface(renderer, option2Surface);
            if (option2Texture) {
                SDL_Rect option2Rect = {
                    messageBox.x + 30,
                    messageBox.y + 130,
                    option2Surface->w,
                    option2Surface->h
                };
                SDL_RenderCopy(renderer, option2Texture, NULL, &option2Rect);
                SDL_DestroyTexture(option2Texture);
            }
            SDL_FreeSurface(option2Surface);
        }

        // Render instructions
        const char* instructions = "Press 1 or 2 to select, Enter to confirm";
        SDL_Surface* instrSurface = TTF_RenderText_Solid(font, instructions, textColor);
        if (instrSurface) {
            SDL_Texture* instrTexture = SDL_CreateTextureFromSurface(renderer, instrSurface);
            if (instrTexture) {
                SDL_Rect instrRect = {
                    messageBox.x + (messageBox.w - instrSurface->w) / 2,
                    messageBox.y + messageBox.h - 50,
                    instrSurface->w,
                    instrSurface->h
                };
                SDL_RenderCopy(renderer, instrTexture, NULL, &instrRect);
                SDL_DestroyTexture(instrTexture);
            }
            SDL_FreeSurface(instrSurface);
        }

        SDL_RenderPresent(renderer);

        // Handle input
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        choice = 1;
                        break;
                    case SDLK_2:
                        choice = 2;
                        break;
                    case SDLK_RETURN:
                        if (choice > 0) {
                            quit = true;
                        }
                        break;
                    case SDLK_ESCAPE:
                        choice = 0;
                        quit = true;
                        break;
                }
            }
        }
        SDL_Delay(10);
    }

    TTF_CloseFont(font);
    SDL_DestroyTexture(currentScreen);

    // Process the choice
    if (choice == 1) {
        // Remove nearest trap
        int playerTileX = (playerRect.x + playerRect.w/2) / TILE_SIZE;
        int playerTileY = (playerRect.y + playerRect.h/2) / TILE_SIZE;
        removeTrapNearPosition(playerRect.x, playerRect.y);
        hasChosenDeathOption = true;
        showMessage("Nearest trap removed!");
        return true;
    }
    else if (choice == 2) {
        // Give freeze ability
        hasFreezeAbility = true;
        hasChosenDeathOption = true;
       // freezeDuration = 10000; // 10 seconds in milliseconds
        showMessage("Press F to freeze the monster for 10 seconds!");
        return true;
    }

    return false;
}

void Game::freezeMonster() {
    if (hasFreezeAbility && !isMonsterFrozen) {
        isMonsterFrozen = true;
        freezeStartTime = SDL_GetTicks();
        hasFreezeAbility = false; // Use up the ability
        showMessage("Monster frozen for 10 seconds!");
    }
}

void Game::updateMonsterFreezeState() {
    if (isMonsterFrozen) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - freezeStartTime >= freezeDuration) {
            isMonsterFrozen = false;
            showMessage("Monster unfrozen!");
        }
    }
}
