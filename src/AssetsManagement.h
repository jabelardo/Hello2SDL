//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_ASSETSMANAGEMENT_H
#define HELLO2SDL_ASSETSMANAGEMENT_H

#include "Game.h"

struct GameContext;
struct GameMemory;
struct SDL_Renderer;
struct SDL_Texture;

bool loadTexture(const char *textureName, const char *filename, const char* resourcePath,
                 SDL_Renderer *renderer, GameContext *gameContext, GameMemory* gameMemory);

SDL_Texture * getTexture(const char *textureName, GameContext *gameContext);

bool unloadTexture(const char *textureName, GameContext *gameContext);

bool loadSound(const char *soundName, SoundType type, const char *fileName, const char* resourcePath,
               GameContext *gameContext, GameMemory* gameMemory);

void playSound(const char *soundName, GameContext *gameContext, int loops = 0);

void playMusic(const char *soundName, GameContext *gameContext, int loops = 0);

bool unloadSound(const char *soundName, SoundType type, GameContext *gameContext);

#endif //HELLO2SDL_ASSETSMANAGEMENT_H
