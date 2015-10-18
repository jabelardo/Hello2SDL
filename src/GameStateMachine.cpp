//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "GameStateMachine.h"

void
GameStateMachine::pushState(GameState *state) {
  assert(SDL_arraysize(gameStates) > currentGameState + 1);
  gameStates[++currentGameState] = state;
}

void
GameStateMachine::changeState(GameState *state) {
  if (currentGameState > -1) {
    if (gameStates[currentGameState]->getStateId() == state->getStateId()) {
      return;
    }
    popState();
  }
  pushState(state);
}

void
GameStateMachine::popState() {
  if (currentGameState > -1) {
    gameStates[currentGameState--] = 0;
  }
}

void
GameStateMachine::render(SDL_Renderer *renderer) {
  if (currentGameState > -1) {
    gameStates[currentGameState]->render(renderer);
  }
}

void
GameStateMachine::update(GameContext *gameContext) {
  if (currentGameState > -1) {
    gameStates[currentGameState]->update(gameContext);
  }
}

void
GameStateMachine::clearStates() {
  gameStates[0] = 0;
  gameStates[1] = 0;
  currentGameState = -1;
}
