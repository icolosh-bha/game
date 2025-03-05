#ifndef game_hpp
#define game_hpp
#include <C:\Users\hangu\Downloads\SDL2-devel-2.28.5-mingw\SDL2-2.28.5\x86_64-w64-mingw32\include\SDL2\SDL.h>
#include<stdio.h>

class Game{
public:
    Game();
    ~Game();
    void init(const char* titile, int xpos,int ypos, int width,int height, bool fullscreen);
  void handleEvents();
  void update();
  void render();
  void clean();
  bool running(){ return isRunning;};
private:
    bool isRunning;
        SDL_Window *window;
        SDL_Renderer *renderer;


};
#endif /* game_hpp*/
