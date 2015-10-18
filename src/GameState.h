//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_GAMESTATE_H
#define HELLO2SDL_GAMESTATE_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "GameStateId.h"
#include "UserInput.h"
#include "GameContext.h"

class GameState {

public:
  virtual bool init(GameContext* gameContext) = 0;
  virtual ~GameState() {};
  virtual void update(GameContext* gameContext) = 0;
  virtual void render(SDL_Renderer* renderer) = 0;
  virtual GameStateId getStateId() const = 0;
};


#endif //HELLO2SDL_GAMESTATE_H
