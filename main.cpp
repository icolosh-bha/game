#include "menu.h"
#include "soundManager.h"
#include "Game.hpp"
#include "map.h"

const std::string SAVE_FILE = "save.dat";

int main(int argc, char** argv) {
    const int W = 800, H = 640;
    SoundManager::get().init();
    SoundManager::get().playMusic("assets/music/background.mp3");

    Menu menu(W, H);
    menu.initFont("arial.ttf", 24);

    Game game;
    menu.setContinueEnabled(game.hasSaveFile(SAVE_FILE));

    bool inMenu = true;
    while (game.running()) {
        if (inMenu) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) return 0;
                menu.handleEvent(e);
            }

            switch (menu.update()) {
                case MenuOption::START:
                    {game.init("Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, false);

                    // 🌟 Chọn level ở đây:
                    int selectedLevel = game.showLevelSelectMenu();
                    char mapFilename[50];
                    sprintf(mapFilename, "maps/level%d.map", selectedLevel);
                 game.getMap()->loadFromFile(mapFilename);



                    inMenu = false;
                    SoundManager::get().playMusic("assets/music/game.mp3");
                    break;
                    }
                case MenuOption::CONTINUE:
                    game.init("Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, false);
                    game.loadGameState(SAVE_FILE);
                    inMenu = false;
                    SoundManager::get().playMusic("assets/music/game.mp3");
                    break;

                case MenuOption::TOGGLE_SOUND:
                    SoundManager::get().toggleMute();
                    menu.setSoundLabel(SoundManager::get().isMuted() ? "Sound: OFF" : "Sound: ON");
                    menu.resetResult();
                    break;

                default: break;
            }

            menu.render(Game::renderer);
            SDL_Delay(16);
        } else {
            game.handleEvents();
            game.update();
            game.render();

            // Auto-save logic
            static int frameCount = 0;
            if (++frameCount >= 300) {
                game.saveGameState(SAVE_FILE);
                frameCount = 0;
            }
        }
    }

    SoundManager::get().cleanup();
    game.clean();
    return 0;
}
