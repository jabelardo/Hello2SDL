//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

#include "TileMap.h"

struct PlayState {
  TileMap *tileMap;
  Entity *enemy;
};

#endif //HELLO2SDL_PLAYSTATE_H
