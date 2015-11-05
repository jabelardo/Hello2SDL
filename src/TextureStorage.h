//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_TEXTURESTORAGE_H
#define HELLO2SDL_TEXTURESTORAGE_H

struct GameContext;
struct GameMemory;
struct SDL_Renderer;
struct SDL_Texture;

bool loadTexture(const char *textureName, const char *filename, const char* resourcePath,
                 SDL_Renderer *renderer, GameContext *gameContext, GameMemory* gameMemory);

SDL_Texture * getTexture(const char *textureName, GameContext *gameContext);

bool unloadTexture(const char *textureName, GameContext *gameContext);

#endif //HELLO2SDL_TEXTURESTORAGE_H
