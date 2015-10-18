//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "MenuState.h"
#include "TextureId.h"
#include "MemoryPartitionPlacementNew.h"

bool
MenuState::init(GameContext* gameContext) {

  if (!gameContext->functions.loadTexture(PLAY_BUTTON, "button.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(EXIT_BUTTON, "exit.png", gameContext->renderer)) {
    return false;
  }
  SDL_Texture *playButton = gameContext->functions.getTexture(PLAY_BUTTON);
  if (!playButton) {
    return false;
  }
  SDL_Texture *exitButton = gameContext->functions.getTexture(EXIT_BUTTON);
  if (!exitButton) {
    return false;
  }

  menuButtons[0] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton({playButton, {100, 100}, 400, 100, 3, 1, 1}, menuToPlay);

  menuButtons[1] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton({exitButton, {100, 300}, 400, 100, 3, 1, 1}, exitFromGame);

  return true;
}

void
MenuState::update(GameContext* gameContext) {
  for (auto &menuButton : menuButtons) {
    menuButton->update(gameContext);
  }
}

void
MenuState::render(SDL_Renderer* renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton->draw(renderer);
  }
}

GameStateId
MenuState::getStateId() const {
  return MENU_STATE;
}

void
MenuState::menuToPlay(GameContext* gameContext) {
  gameContext->stateChange = RESTART_PLAY;
}

void
MenuState::exitFromGame(GameContext* gameContext) {
  gameContext->stateChange = EXIT_FROM_GAME;
}
