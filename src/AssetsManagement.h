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

bool loadSound(const char *soundName,const char *fileName, const char* resourcePath,
               GameContext *gameContext, GameMemory* gameMemory);

bool loadMusic(const char *soundName,const char *fileName, const char* resourcePath,
               GameContext *gameContext, GameMemory* gameMemory);

bool unloadTexture(const char *textureName, GameContext *gameContext);

bool unloadSound(const char *soundName, GameContext *gameContext);

bool unloadMusic(const char *soundName, GameContext *gameContext);

SDL_Texture * getTexture(const char *textureName, GameContext *gameContext);

void playSound(const char *soundName, GameContext *gameContext, int loops = 0);

void playMusic(const char *soundName, GameContext *gameContext, int loops = 0);

#endif //HELLO2SDL_ASSETSMANAGEMENT_H
