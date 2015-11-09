//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "Game.h"
#include "SharedDefinitions.h"

#include "AnimatedGraphic.cpp"
#include "AssetsManagement.cpp"
#include "Entity.cpp"
#include "GameOverMenu.cpp"
#include "MainMenu.cpp"
#include "MemoryPartition.cpp"
#include "MenuButton.cpp"
#include "PauseMenu.cpp"
#include "PlayState.cpp"
#include "RenderUtils.cpp"
#include "ScrollingBackground.cpp"
#include "TileMap.cpp"

void
updateGame(GameContext *gameContext, UserInput* userInput, GameMemory* gameMemory) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      if (userInput->back.endedDown) {
        gameContext->stateChange = PAUSE_MENU;
      } else {
        updatePlayState(gameContext->playState, gameContext, userInput, gameMemory);
      }
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
      renderPlayState(gameContext->playState, gameContext, renderer);
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
processStateChange(GameContext *gameContext, UserInput* userInput, SDL_Renderer *renderer, GameMemory* gameMemory) {
  switch (gameContext->stateChange) {
    case NO_CHANGE: {
      break;
    }
    case START_PLAY: {
//      SDL_ShowCursor(SDL_DISABLE);
      *userInput = {};
      startGame(gameContext->playState, gameContext, renderer, gameMemory);
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case EXIT_FROM_GAME: {
      userInput->shouldQuit = true;
      break;
    }
    case MAIN_MENU: {
//      SDL_ShowCursor(SDL_ENABLE);
      *userInput = {};
      gameContext->currentState = MAIN_MENU_STATE;
      break;
    }
    case RESUME_PLAY: {
//      SDL_ShowCursor(SDL_DISABLE);
      *userInput = {};
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case PAUSE_MENU: {
//      SDL_ShowCursor(SDL_ENABLE);
      *userInput = {};
      gameContext->currentState = PAUSE_MENU_STATE;
      break;
    }
    case GAME_OVER: {
//      SDL_ShowCursor(SDL_ENABLE);
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

  gameContext->resourcePath = platformConfig->resourcePath;
  gameContext->dtPerFrame = platformConfig->dtPerFrame;
  gameContext->gameWidth = platformConfig->screenWidth;
  gameContext->gameHeight = platformConfig->screenHeight;

  if (!gameMemory->isInitialized) {
    gameContext = RESERVE_MEMORY(&gameMemory->permanentMemory, GameContext);

    gameContext->mainMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, MainMenu);
    if (!initMainMenu(gameContext->mainMenu, gameContext, renderer, gameMemory)) {
      return -1;
    }
    gameContext->playState = RESERVE_MEMORY(&gameMemory->permanentMemory, PlayState);
    if (!initPlayState(gameContext->playState, gameContext, renderer, gameMemory)) {
      return -1;
    }
    gameContext->pauseMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, PauseMenu);
    if (!initPauseMenu(gameContext->pauseMenu, gameContext, renderer, gameMemory)) {
      return -1;
    }
    gameContext->gameOverMenu = RESERVE_MEMORY(&gameMemory->permanentMemory, GameOverMenu);
    if (!initGameOverMenu(gameContext->gameOverMenu, gameContext, renderer, gameMemory)) {
      return -1;
    }
    gameContext->currentState = MAIN_MENU_STATE;
    gameContext->scrollSpeed = 1;
    gameContext->pixelsPerMt = 10;
    gameMemory->isInitialized = true;

    // add some sound effects - TODO move to better place
    loadSound("music1", SOUND_MUSIC, "DST_ElectroRock.ogg", gameContext->resourcePath, gameContext, gameMemory);
    loadSound("explode", SOUND_SFX, "boom.wav", gameContext->resourcePath, gameContext, gameMemory);
    loadSound("shoot", SOUND_SFX, "phaser.wav", gameContext->resourcePath, gameContext, gameMemory);

    playMusic("music1", gameContext, -1);
  }
  gameContext->stateChange = NO_CHANGE;

  updateGame(gameContext, userInput, gameMemory);
  processStateChange(gameContext, userInput, renderer, gameMemory);
  renderGame(gameContext, renderer);

  return 0;
}
