//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "Game.h"
#include "SharedDefinitions.h"

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
updateGame(GameContext *gameContext, UserInput* userInput, GameMemory* gameMemory) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      updatePlayState(gameContext->playState, gameContext, userInput, gameMemory);
      break;
    }
    case MAIN_MENU_STATE: {
      updateMainMenu(gameContext->mainMenu, gameContext, userInput);
      break;
    }
    case PAUSE_MENU_STATE: {
      updatePauseMenu(gameContext->pauseMenu, gameContext, userInput);
      break;
    }
    case GAME_OVER_STATE: {
      updateGameOverMenu(gameContext->gameOverMenu, gameContext, userInput);
      break;
    }
    case BETWEEN_LEVEL_STATE:{
      break;
    }
  }
}

void
renderGame(GameContext *gameContext, SDL_Renderer *renderer) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      renderPlayState(gameContext->playState, renderer);
      break;
    }
    case MAIN_MENU_STATE: {
      renderMainMenu(gameContext->mainMenu, renderer);
      break;
    }
    case PAUSE_MENU_STATE: {
      renderPauseMenu(gameContext->pauseMenu, renderer);
      break;
    }
    case GAME_OVER_STATE: {
      renderGameOverMenu(gameContext->gameOverMenu, renderer);
      break;
    }
    case BETWEEN_LEVEL_STATE:{
      break;
    }
  }
}

void
processStateChange(GameContext *gameContext, UserInput* userInput) {
  switch (gameContext->stateChange) {
    case NO_CHANGE: {
      break;
    }
    case START_PLAY: {
      *userInput = {};
      startGame(gameContext->playState, gameContext);
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case EXIT_FROM_GAME: {
      userInput->shouldQuit = true;
      break;
    }
    case MAIN_MENU: {
      *userInput = {};
      gameContext->currentState = MAIN_MENU_STATE;
      break;
    }
    case RESUME_PLAY: {
      *userInput = {};
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case PAUSE_MENU: {
      *userInput = {};
      gameContext->currentState = PAUSE_MENU_STATE;
      break;
    }
    case GAME_OVER: {
      *userInput = {};
      gameContext->currentState = GAME_OVER_STATE;
      break;
    }
    case BETWEEN_LEVEL:{
      break;
    }
  }
}

extern "C" int
gameUpdateAndRender(PlatformConfig *platformConfig, UserInput* userInput, GameMemory* gameMemory,
                    SDL_Renderer *renderer) {

  GameContext *gameContext = (GameContext *) gameMemory->permanentMemory.base;

  if (!gameMemory->isInitialized) {
    gameContext = RESERVE_MEMORY(&gameMemory->permanentMemory, GameContext);

    gameContext->mainMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, MainMenu);
    if (!initMainMenu(gameContext->mainMenu, gameContext, renderer, gameMemory, platformConfig)) {
      return -1;
    }
    gameContext->playState = RESERVE_MEMORY(&gameMemory->permanentMemory, PlayState);
    if (!initPlayState(gameContext->playState, gameContext, renderer, gameMemory, platformConfig)) {
      return -1;
    }
    gameContext->pauseMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, PauseMenu);
    if (!initPauseMenu(gameContext->pauseMenu, gameContext, renderer, gameMemory, platformConfig)) {
      return -1;
    }
    gameContext->gameOverMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, GameOverMenu);
    if (!initGameOverMenu(gameContext->gameOverMenu, gameContext, renderer, gameMemory, platformConfig)) {
      return -1;
    }
    gameContext->currentState = MAIN_MENU_STATE;
    gameContext->gameWidth = platformConfig->screenWidth;
    gameContext->gameHeight = platformConfig->screenHeight;
    gameContext->scrollSpeed = 1;
    gameMemory->isInitialized = true;
  }
  gameContext->stateChange = NO_CHANGE;
  updateGame(gameContext, userInput, gameMemory);
  processStateChange(gameContext, userInput);
  renderGame(gameContext, renderer);

  return 0;
}
