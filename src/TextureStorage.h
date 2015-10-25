//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_TEXTURESTORAGE_H
#define HELLO2SDL_TEXTURESTORAGE_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#include "Game.h"

#else
#include <SDL_render.h>
#endif

bool loadTexture(const char *textureName, const char *filename, SDL_Renderer *renderer,
                 GameContext *gameContext);

SDL_Texture * getTexture(const char *textureName, GameContext *gameContext);

bool unloadTexture(const char *textureName, GameContext *gameContext);

#endif //HELLO2SDL_TEXTURESTORAGE_H
