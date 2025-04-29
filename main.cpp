// main.cpp
#include "Game.hpp"
#include "menu.h"
#include "soundManager.h"

// Define save file path
const std::string SAVE_FILE = "save.dat";

int main(int argc, char** argv){
  const int W=800, H=600;
  SoundManager::get().init();
  SoundManager::get().playMusic("assets/music/background.mp3");

  Menu menu(W,H);

  // Initialize font for the menu
  menu.initFont("arial.ttf", 24);

  // enable Continue if save exists:
  Game game;
  menu.setContinueEnabled(game.hasSaveFile(SAVE_FILE));

  bool inMenu=true;
  while(game.running()){
    if(inMenu){
      SDL_Event e;
      while(SDL_PollEvent(&e)){
        if(e.type==SDL_QUIT) return 0;
        menu.handleEvent(e);
      }
      switch(menu.update()){
        case MenuOption::START:
          game.init("Maze",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, W,H,false);
          inMenu=false;
          SoundManager::get().playMusic("assets/music/game.mp3");
          break;
        case MenuOption::CONTINUE:
          game.init("Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, false);
          game.loadGameState(SAVE_FILE); // Load the saved game state
          inMenu=false;
          SoundManager::get().playMusic("assets/music/game.mp3");
          break;
        case MenuOption::TOGGLE_SOUND:
          SoundManager::get().toggleMute();
          menu.setSoundLabel(SoundManager::get().isMuted() ? "Sound: OFF" : "Sound: ON");
          break;
        default: break;
      }
      menu.render(Game::renderer);
      SDL_Delay(16);
    }
    else {
      game.handleEvents();
      game.update();
      game.render();

      // Auto-save game state when player has moved or collected items
      static int lastX = 0, lastY = 0;
      // We can't access playerRect directly, so we'll save periodically
      static int frameCount = 0;
      if (++frameCount >= 300) { // Save every ~5 seconds (assuming 60 FPS)
        game.saveGameState(SAVE_FILE);
        frameCount = 0;
      }
    }
  }

  SoundManager::get().cleanup();
  game.clean();
  return 0;
}
