// soundManager.h
#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>

class SoundManager {
public:
  static SoundManager& get();
  bool init();                     // Mix_OpenAudio
  bool playMusic(const std::string&);
  void playEffect(const std::string&);
  void toggleMute();
  bool isMuted() const { return muted; }
  void cleanup();                  // free + Mix_CloseAudio

private:
  SoundManager();
  ~SoundManager();
  bool muted;
  Mix_Music* bg=nullptr;
  std::string currentFile;  // track last loaded music file
};
