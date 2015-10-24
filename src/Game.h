//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#ifdef __APPLE__

#include <SDL2/SDL_render.h>

#else
#include <SDL_render.h>
#endif

#include "UserInput.h"
#include "GameContext.h"

#ifdef __cplusplus
extern "C" {
#endif

int gameUpdateAndRender(GameContext *gameContext);


#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_GAME_H
