#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "game.hpp"
#include <C:\Users\hangu\Downloads\SDL2_image-devel-2.8.8-mingw\SDL2_image-2.8.8\x86_64-w64-mingw32\include\SDL2\SDL_image.h>
#include <iostream>
#include <string>

class TextureManager {
public:
    static SDL_Renderer* renderer;

    static SDL_Texture* LoadTexture(const char* fileName) {
        std::cout << "Attempting to load texture: " << fileName << std::endl;
        
        // Load surface từ file PNG
        SDL_Surface* surface = IMG_Load(fileName);
        if (!surface) {
            std::cout << "Failed to load surface: " << fileName << " - " << IMG_GetError() << std::endl;
            return nullptr;
        }
        
        std::cout << "Surface loaded successfully. Format: " << SDL_GetPixelFormatName(surface->format->format) 
                  << ", Size: " << surface->w << "x" << surface->h << std::endl;

        // Chuyển đổi surface sang định dạng RGBA32
        SDL_Surface* optimizedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);

        if (!optimizedSurface) {
            std::cout << "Failed to optimize surface: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        // Kiểm tra tên file để quyết định có xóa nền trắng hay không
        std::string filename = fileName;
        bool keepWhiteBackground = (filename == "trap.png" || filename == "key.png" || filename == "goal.png");

        if (!keepWhiteBackground) {
            // Xóa nền trắng bằng cách đặt alpha = 0 cho các pixel màu trắng
            SDL_LockSurface(optimizedSurface);
            Uint32* pixels = (Uint32*)optimizedSurface->pixels;
            int pixelCount = optimizedSurface->w * optimizedSurface->h;
            
            for (int i = 0; i < pixelCount; i++) {
                Uint8 r, g, b, a;
                SDL_GetRGBA(pixels[i], optimizedSurface->format, &r, &g, &b, &a);
                
                // Nếu pixel gần với màu trắng (RGB đều > 250)
                if (r > 250 && g > 250 && b > 250) {
                    pixels[i] = SDL_MapRGBA(optimizedSurface->format, r, g, b, 0); // đặt alpha = 0
                }
            }
            
            SDL_UnlockSurface(optimizedSurface);
        }

        // Tạo texture từ surface đã xử lý
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, optimizedSurface);
        SDL_FreeSurface(optimizedSurface);

        if (!texture) {
            std::cout << "Failed to create texture: " << fileName << " - " << SDL_GetError() << std::endl;
            return nullptr;
        }

        // Thiết lập blend mode cho texture
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        
        std::cout << "Successfully loaded texture: " << fileName << std::endl;
        return texture;
    }
};

#endif
