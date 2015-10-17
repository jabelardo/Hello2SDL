//
// Created by Jose Gutierrez on 10/15/15.
//

#include <SDL2/SDL_render.h>
#include <assert.h>
#include "GameStateMachine.h"

void
GameStateMachine::pushState(GameState* state, GameContext* gameContext) {
  assert(SDL_arraysize(gameStates) > currentGameState + 1);
  gameStates[++currentGameState] = state;
  state->onEnter(gameContext);
}

void
GameStateMachine::changeState(GameState* state, GameContext* gameContext) {
  if (currentGameState > -1) {
    if (gameStates[currentGameState]->getStateId() == state->getStateId()) {
      return;
    }
    popState(gameContext);
  }
  pushState(state, gameContext);
}

void
GameStateMachine::popState(GameContext* gameContext) {
  if (currentGameState > -1) {
    if (gameStates[currentGameState]->onExit(gameContext)) {
      --currentGameState;
    }
  }
}

void
GameStateMachine::render(SDL_Renderer* renderer) {
  if (currentGameState > -1) {
    gameStates[currentGameState]->render(renderer);
  }
}

void
GameStateMachine::update(GameContext* gameContext) {
  if (currentGameState > -1) {
    gameStates[currentGameState]->update(gameContext);
  }
}

void GameStateMachine::clear(GameContext* gameContext) {
  while (currentGameState > -1) {
    gameStates[currentGameState--]->onExit(gameContext);
  }
}
