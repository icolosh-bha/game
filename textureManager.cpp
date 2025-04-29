// textureManager.cpp
#include "TextureManager.h"
#include <iostream>

SDL_Renderer* TextureManager::renderer = nullptr;

SDL_Texture* TextureManager::LoadTexture(const char* fileName) {
    SDL_Surface* tempSurface = IMG_Load(fileName);
    if (!tempSurface) {
        std::cout << "Failed to load image " << fileName << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    // Set white color (255, 255, 255) as transparent
    SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, 255, 255, 255));
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    if (!tex) {
        std::cout << "Failed to create texture from " << fileName << ": " << SDL_GetError() << std::endl;
    } else {
        // Enable alpha blending for the texture
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    }
    
    SDL_FreeSurface(tempSurface);
    return tex;
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest) {
    SDL_RenderCopy(Game::renderer, tex, &src, &dest);
}
