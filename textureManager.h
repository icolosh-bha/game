#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <C:\Users\hangu\Downloads\SDL2_image-devel-2.8.4-mingw\SDL2_image-2.8.4\x86_64-w64-mingw32\include\SDL2\SDL_image.h>
#include "game.hpp"
#include <iostream>

class TextureManager {
public:
    static SDL_Renderer* renderer;
    static SDL_Texture* LoadTexture(const char* texture);
    static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest);
};

#endif

