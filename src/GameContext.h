//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAMECONTEXT_H
#define HELLO2SDL_GAMECONTEXT_H

#include "UserInput.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (LoadTextureFunc)(int textureId, const char* fileName, SDL_Renderer *renderer);
typedef SDL_Texture* GetTextureFunc(int textureId);
typedef bool UnloadTextureFunc(int textureId);

typedef struct PlatformFunctions {
  LoadTextureFunc* loadTexture;
  GetTextureFunc* getTexture;
  UnloadTextureFunc* unloadTexture;
} PlatformFunctions;

typedef struct GameContext {
  bool isInitialized;
  UserInput *userInput;
  SDL_Renderer *renderer;
  PlatformFunctions functions;
} GameContext;

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_GAMECONTEXT_H
