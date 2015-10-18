//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_GAMESTATEMACHINE_H
#define HELLO2SDL_GAMESTATEMACHINE_H

#include "GameState.h"

class GameStateMachine {
public:
  void pushState(GameState* state);

  void changeState(GameState* state);

  void popState();

  void update(GameContext* gameContext);

  void render(SDL_Renderer* renderer);

  void clearStates();

private:
  int currentGameState = -1;
  GameState* gameStates[2];
};


#endif //HELLO2SDL_GAMESTATEMACHINE_H
