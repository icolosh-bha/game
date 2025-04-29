// menu.h
#pragma once
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

enum class MenuOption {
  NONE,
  START,
  CONTINUE,
  TOGGLE_SOUND
};

class Menu {
public:
  Menu(int screenW, int screenH);
  ~Menu();

  // call once per SDL_Event
  void handleEvent(const SDL_Event& e);

  // call every frame until returns != NONE
  MenuOption update();

  // draw full menu
  void render(SDL_Renderer* renderer);

  // enable/disable Continue button
  void setContinueEnabled(bool e);

  // Initialize font
  bool initFont(const char* fontPath, int fontSize);
void setSoundLabel(const std::string& text);
private:
  struct Button {
    SDL_Rect  rect;
    MenuOption id ;
    bool      hover;
    bool      enabled;
    std::string label;
  };
  std::vector<Button> buttons;
  int W, H;
  MenuOption result;
  TTF_Font* font;

  // Helper function to render text
  void renderText(SDL_Renderer* renderer, const char* text, SDL_Rect* rect, SDL_Color color);
};
