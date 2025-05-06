// menu.cpp
#include "menu.h"
#include "textureManager.h" // Thêm include cho TextureManager
#include <iostream>

Menu::Menu(int screenW, int screenH)
 : W(screenW), H(screenH), result(MenuOption::NONE), font(nullptr), backgroundTexture(nullptr)
{
  int bw = 280;
  int bh = 60;
  int gap = 70;                       // space between buttons
  int totalHeight = bh * 3 + gap * 2; // total height of buttons + gaps
  int y0 = (H - totalHeight) / 2;    // vertical centering
  int step = bh + gap;
  buttons = {
  {{(W - bw) / 2, y0 + 0 * step, bw, bh}, MenuOption::START,        false, true,  "Start"},
  {{(W - bw) / 2, y0 + 1 * step, bw, bh}, MenuOption::CONTINUE,     false, false, "Continue"},
  {{(W - bw) / 2, y0 + 2 * step, bw, bh}, MenuOption::TOGGLE_SOUND, false, true,  "Sound"},
};


  // Initialize TTF
  if (TTF_Init() == -1) {
    std::cout << "TTF_Init failed: " << TTF_GetError() << std::endl;
  }

  // Tải hình nền
  backgroundTexture = TextureManager::LoadTexture("background.png");
  if (backgroundTexture == NULL) {
    std::cout << "Failed to load menu background: " << SDL_GetError() << std::endl;
  }
}

Menu::~Menu() {
  if (font) {
    TTF_CloseFont(font);
    font = nullptr;
  }

  // Giải phóng texture hình nền
  if (backgroundTexture) {
    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = nullptr;
  }

  TTF_Quit();
}

/**
 * Initialize font for text rendering
 * @param fontPath Path to the font file
 * @param fontSize Size of the font
 * @return true if successful, false otherwise
 */
bool Menu::initFont(const char* fontPath, int fontSize) {
  if (font) {
    TTF_CloseFont(font);
  }

  font = TTF_OpenFont(fontPath, fontSize);
  if (!font) {
    std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    return false;
  }
  return true;
}

void Menu::setContinueEnabled(bool e) {
  buttons[1].enabled = e;
}
void Menu::setSoundLabel(const std::string& text) {
    for (auto& btn : buttons) {
        if (btn.id == MenuOption::TOGGLE_SOUND) {
            btn.label = text;
        }
    }
}
void Menu::handleEvent(const SDL_Event& e) {
  if (e.type==SDL_MOUSEMOTION) {
    SDL_Point p = {e.motion.x, e.motion.y};
    for(auto& b : buttons) {
      b.hover = b.enabled && SDL_PointInRect(&p, &b.rect);
    }
  }
  else if(e.type==SDL_MOUSEBUTTONDOWN) {
    SDL_Point p = {e.button.x, e.button.y};
    for(auto& b : buttons) {
      if(b.enabled && SDL_PointInRect(&p, &b.rect)) {
        result = b.id;
      }
    }
  }
}

void Menu::resetResult() { result = MenuOption::NONE; }

MenuOption Menu::update() {
  return result;
}

/**
 * Render text with the given font
 * @param renderer SDL renderer
 * @param text Text to render
 * @param rect Rectangle to render text in (centered)
 * @param color Text color
 */
void Menu::renderText(SDL_Renderer* renderer, const char* text, SDL_Rect* rect, SDL_Color color) {
  if (!font) {
    return;
  }

  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  if (!surface) {
    std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
    return;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    SDL_FreeSurface(surface);
    return;
  }

  // Center text in the rectangle
  SDL_Rect textRect;
  textRect.w = surface->w;
  textRect.h = surface->h;
  textRect.x = rect->x + (rect->w - textRect.w) / 2;
  textRect.y = rect->y + (rect->h - textRect.h) / 2;

  SDL_RenderCopy(renderer, texture, NULL, &textRect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void Menu::render(SDL_Renderer* R) {
  // Xóa màn hình với màu đen
  SDL_SetRenderDrawColor(R, 0, 0, 0, 255);
  SDL_RenderClear(R);

  // Vẽ hình nền nếu có
  if (backgroundTexture) {
    SDL_Rect destRect = {0, 0, W, H}; // Vẽ hình nền kích thước toàn màn hình
    SDL_RenderCopy(R, backgroundTexture, NULL, &destRect);
  }

  // Vẽ các nút
  for(auto& b:buttons){
    // Enhanced hover effect with gradient and glow
    SDL_Color baseColor, textColor;

    if (!b.enabled) {
      // Disabled button
      baseColor = {50, 50, 50, 255};
      textColor = {120, 120, 120, 255};
    } else if (b.hover) {
      // Hover effect - brighter with glow
      baseColor = {180, 180, 255, 255};
      textColor = {255, 255, 255, 255};

      // Draw glow effect (larger rectangle with alpha)
      SDL_SetRenderDrawBlendMode(R, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(R, 200, 200, 255, 100);
      SDL_Rect glowRect = b.rect;
      glowRect.x -= 5;
      glowRect.y -= 5;
      glowRect.w += 10;
      glowRect.h += 10;
      SDL_RenderFillRect(R, &glowRect);
      SDL_SetRenderDrawBlendMode(R, SDL_BLENDMODE_NONE);
    } else {
      // Normal button
      baseColor = {100, 100, 180, 255};
      textColor = {220, 220, 220, 255};
    }

    // Draw button background
    SDL_SetRenderDrawColor(R, baseColor.r, baseColor.g, baseColor.b, baseColor.a);
    SDL_RenderFillRect(R, &b.rect);

    // Draw button border
    SDL_SetRenderDrawColor(R, textColor.r, textColor.g, textColor.b, textColor.a);
    SDL_RenderDrawRect(R, &b.rect);

    // Render button text
    if (font) {
      renderText(R, b.label.c_str(), &b.rect, textColor);
    }
  }
  SDL_RenderPresent(R);
}
