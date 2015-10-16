//
// Created by Jose Gutierrez on 10/15/15.
//

#include <SDL2/SDL_render.h>
#include <assert.h>
#include "GameStateMachine.h"
#include "TextureManager.h"
#include "InputHandler.h"

void
GameStateMachine::pushState(std::unique_ptr<GameState> state, TextureManager *textureManager,
                            SDL_Renderer *renderer) {
  assert(state);
  gameStates.push_back(std::move(state));
  gameStates.back()->onEnter(textureManager, renderer);
}

void
GameStateMachine::changeState(std::unique_ptr<GameState> state, TextureManager *textureManager,
                              SDL_Renderer *renderer) {
  assert(state);
  if (!gameStates.empty()) {
    if (gameStates.back()->getStateId() == state->getStateId()) {
      return;
    }
    if (gameStates.back()->onExit(textureManager)) {
      gameStates.pop_back();
    }
  }
  pushState(std::move(state), textureManager, renderer);
}

void
GameStateMachine::popState(TextureManager *textureManager) {
  if (!gameStates.empty()) {
    if (gameStates.back()->onExit(textureManager)) {
      gameStates.pop_back();
    }
  }
}

void
GameStateMachine::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (!gameStates.empty()) {
    gameStates.back()->render(textureManager, renderer);
  }
}

void
GameStateMachine::update(InputHandler *inputHandler) {
  if (!gameStates.empty()) {
    gameStates.back()->update(inputHandler);
  }
}
