
// soundManager.cpp
#include "soundManager.h"
#include <iostream>

SoundManager& SoundManager::get() {
  static SoundManager mgr;
  return mgr;
}

SoundManager::SoundManager():muted(false){}
SoundManager::~SoundManager(){}

bool SoundManager::init() {
  if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0){
    std::cout<<"SDL_mixer init error:"<<Mix_GetError()<<"\n";
    return false;
  }
  return true;
}

bool SoundManager::playMusic(const std::string& f){
  if(bg) Mix_FreeMusic(bg);
  bg = Mix_LoadMUS(f.c_str());
  if(!bg) { std::cout<<"LoadMusic error:"<<Mix_GetError()<<"\n"; return false; }
  if(!muted) Mix_PlayMusic(bg,-1);
  return true;
}

void SoundManager::playEffect(const std::string& f){
  if(muted) return;
  Mix_Chunk* c=Mix_LoadWAV(f.c_str());
  if(c){
    Mix_PlayChannel(-1,c,0);
    Mix_FreeChunk(c);
  }
}

void SoundManager::toggleMute(){
  muted = !muted;
  if(muted) Mix_PauseMusic();
  else      Mix_ResumeMusic();
}

void SoundManager::cleanup(){
  if(bg) Mix_FreeMusic(bg);
  Mix_CloseAudio();
}
