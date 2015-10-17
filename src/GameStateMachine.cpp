//
// Created by Jose Gutierrez on 10/15/15.
//

#include <SDL2/SDL_render.h>
#include <assert.h>
#include "GameStateMachine.h"
#include "TextureManager.h"
#include "UserInput.h"

void
GameStateMachine::pushState(GameState& state, TextureManager *textureManager,
                            SDL_Renderer *renderer) {
  gameStates.push_back(state);
  gameStates.back().get().onEnter(textureManager, renderer);
}

void
GameStateMachine::changeState(GameState& state, TextureManager *textureManager,
                              SDL_Renderer *renderer) {
  if (!gameStates.empty()) {
    if (gameStates.back().get().getStateId() == state.getStateId()) {
      return;
    }
    popState(textureManager);
  }
  pushState(state, textureManager, renderer);
}

void
GameStateMachine::popState(TextureManager *textureManager) {
  if (!gameStates.empty()) {
    if (gameStates.back().get().onExit(textureManager)) {
      gameStates.pop_back();
    }
  }
}

void
GameStateMachine::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (!gameStates.empty()) {
    gameStates.back().get().render(textureManager, renderer);
  }
}

void
GameStateMachine::update(UserInput *userInput, SDL_Renderer *renderer) {
  if (!gameStates.empty()) {
    gameStates.back().get().update(userInput, renderer);
  }
}
