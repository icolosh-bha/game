#include "game.hpp"
#include <iostream>
#include <SDL.h>
#include <C:\Users\hangu\Downloads\SDL2_image-devel-2.8.8-mingw\SDL2_image-2.8.8\x86_64-w64-mingw32\include\SDL2\SDL_image.h>

// Hàm kiểm tra file tồn tại (hỗ trợ đường dẫn)
bool fileExists(const char* path) {
    SDL_RWops* file = SDL_RWFromFile(path, "r");
    if (file) {
        SDL_RWclose(file);
        return true;
    }
    return false;
}

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), playerTexture(nullptr) {
    // Khởi tạo các con trỏ về nullptr để tránh lỗi
}

Game::~Game() {
    clean(); // Gọi clean để dọn dẹp tài nguyên khi hủy đối tượng
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems initialized!" << std::endl;
        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            std::cout << "Window created!" << std::endl;
        } else {
            std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Sử dụng accelerated renderer
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nền đen
            std::cout << "Renderer created!" << std::endl;
        } else {
            std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        // Khởi tạo SDL_image
        if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
            std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
            isRunning = false;
            return;
        }

        // Kiểm tra và tải texture từ file
        const char* imagePath = "player.png"; // Đường dẫn tương đối
        if (!fileExists(imagePath)) {
            std::cout << "Image file '" << imagePath << "' not found!" << std::endl;
            isRunning = false;
            return;
        }

        SDL_Surface* surface = IMG_Load(imagePath);
        if (!surface) {
            std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
            isRunning = false;
            return;
        }
        if (SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255)) != 0) {
            std::cout << "Failed to set color key: " << SDL_GetError() << std::endl;
            isRunning = false;
            SDL_FreeSurface(surface);
            return;
        }
        playerTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (playerTexture) {
            float scale=0.25f;
            playerRect.w = static_cast<int>(surface->w*scale); // Lấy chiều rộng thực tế
            playerRect.h = static_cast<int>(surface->h*scale); // Lấy chiều cao thực tế
            playerRect.x = 100;        // Vị trí ban đầu x
            playerRect.y = 100;        // Vị trí ban đầu y
        } else {
            std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
            isRunning = false;
        }
        SDL_FreeSurface(surface);

        isRunning = true;
    } else {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RIGHT:
                playerRect.x += 5; // Di chuyển sang phải
                break;
            case SDLK_LEFT:
                playerRect.x -= 5; // Di chuyển sang trái
                break;
            case SDLK_UP:
                playerRect.y -= 5; // Di chuyển lên
                break;
            case SDLK_DOWN:
                playerRect.y += 5; // Di chuyển xuống
                break;
            }
            break;
        default:
            break;
        }
    }
}

void Game::update() {
    // Giới hạn vị trí sprite trong màn hình (giả sử cửa sổ 800x600)
    if (playerRect.x < 0) playerRect.x = 0;
    if (playerRect.x > 800 - playerRect.w) playerRect.x = 800 - playerRect.w;
    if (playerRect.y < 0) playerRect.y = 0;
    if (playerRect.y > 600 - playerRect.h) playerRect.y = 600 - playerRect.h;
}

void Game::render() {
    SDL_RenderClear(renderer); // Xóa màn hình bằng màu nền
    if (playerTexture) {
        SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect); // Vẽ texture
    }
    SDL_RenderPresent(renderer); // Cập nhật màn hình
}

void Game::clean() {
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    IMG_Quit(); // Dọn dẹp SDL_image
    SDL_Quit();
    std::cout << "Game cleaned" << std::endl;
}
