//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "Game.h"

#include "AnimatedGraphic.cpp"
#include "Entity.cpp"
#include "GameOverMenu.cpp"
#include "MainMenu.cpp"
#include "MemoryPartition.cpp"
#include "MenuButton.cpp"
#include "PauseMenu.cpp"
#include "PlayState.cpp"
#include "RenderUtils.cpp"
#include "TextureStorage.cpp"
#include "TileMap.cpp"

void
updateGame(GameContext *gameContext) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      updatePlayState(gameContext->playState, gameContext);
      break;
    }
    case MAIN_MENU_STATE: {
      updateMainMenu(gameContext->mainMenu, gameContext);
      break;
    }
    case PAUSE_MENU_STATE: {
      updatePauseMenu(gameContext->pauseMenu, gameContext);
      break;
    }
    case GAME_OVER_STATE: {
      updateGameOverMenu(gameContext->gameOverMenu, gameContext);
      break;
    }
    case NOT_INITIALIZED_STATE:{
      assert(!("NOT_INITIALIZED_STATE"));
      break;
    }
  }
}

void
renderGame(GameContext *gameContext) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      renderPlayState(gameContext->playState, gameContext->renderer);
      break;
    }
    case MAIN_MENU_STATE: {
      renderMainMenu(gameContext->mainMenu, gameContext->renderer);
      break;
    }
    case PAUSE_MENU_STATE: {
      renderPauseMenu(gameContext->pauseMenu, gameContext->renderer);
      break;
    }
    case GAME_OVER_STATE: {
      renderGameOverMenu(gameContext->gameOverMenu, gameContext->renderer);
      break;
    }
    case NOT_INITIALIZED_STATE: {
      assert(!("NOT_INITIALIZED_STATE"));
      break;
    }
  }
}

void
processStateChange(GameContext *gameContext) {
  switch (gameContext->stateChange) {
    case NONE: {
      break;
    }
    case START_PLAY: {
      gameContext->userInput = {};
      startGame(gameContext->playState, gameContext);
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case EXIT_FROM_GAME: {
      gameContext->userInput.shouldQuit = true;
      break;
    }
    case MAIN_MENU: {
      gameContext->userInput = {};
      gameContext->currentState = MAIN_MENU_STATE;
      break;
    }
    case RESUME_PLAY: {
      gameContext->userInput = {};
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case PAUSE_MENU: {
      gameContext->userInput = {};
      gameContext->currentState = PAUSE_MENU_STATE;
      break;
    }
    case GAME_OVER: {
      gameContext->userInput = {};
      gameContext->currentState = GAME_OVER_STATE;
      break;
    }
  }
}

extern "C" int
gameUpdateAndRender(GameContext *gameContext) {

  if (gameContext->currentState == NOT_INITIALIZED_STATE) {
    gameContext->mainMenu = (MainMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(MainMenu));
    if (!initMainMenu(gameContext->mainMenu, gameContext)) {
      return -1;
    }
    gameContext->playState = (PlayState *) reserveMemory(&gameContext->permanentMemory, sizeof(PlayState));
    if (!initPlayState(gameContext->playState, gameContext)) {
      return -1;
    }
    gameContext->pauseMenu = (PauseMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(PauseMenu));
    if (!initPauseMenu(gameContext->pauseMenu, gameContext)) {
      return -1;
    }
    gameContext->gameOverMenu = (GameOverMenu *) reserveMemory(&gameContext->permanentMemory,
                                                    sizeof(GameOverMenu));
    if (!initGameOverMenu(gameContext->gameOverMenu, gameContext)) {
      return -1;
    }
    gameContext->currentState = MAIN_MENU_STATE;
  }
  gameContext->stateChange = NONE;
  updateGame(gameContext);
  processStateChange(gameContext);
  renderGame(gameContext);

  return 0;
}
