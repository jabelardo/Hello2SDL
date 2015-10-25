//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "Game.h"

#include "AnimatedGraphic.cpp"
#include "Entity.cpp"
#include "GameOverMenu.cpp"
#include "MainMenu.cpp"
#include "MenuButton.cpp"
#include "PauseMenu.cpp"
#include "PlayState.cpp"
#include "RenderUtils.cpp"
#include "TextureStorage.cpp"
#include "TileMap.cpp"

MainMenu *G_mainMenu;

PlayState *G_playState;

PauseMenu *G_pauseMenu;

GameOverMenu *G_gameOverMenu;

void
startPlay(GameContext *gameContext) {
  gameContext->stateChange = START_PLAY;
}

void
exitFromGame(GameContext *gameContext) {
  gameContext->stateChange = EXIT_FROM_GAME;
}

void
resumePlay(GameContext *gameContext) {
  gameContext->stateChange = RESUME_PLAY;
}

void
mainMenu(GameContext *gameContext) {
  gameContext->stateChange = MAIN_MENU;
}

void
updateGame(GameContext *gameContext) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      updatePlayState(G_playState, gameContext);
      break;
    }
    case MAIN_MENU_STATE: {
      updateMainMenu(G_mainMenu, gameContext);
      break;
    }
    case PAUSE_MENU_STATE: {
      updatePauseMenu(G_pauseMenu, gameContext);
      break;
    }
    case GAME_OVER_STATE: {
      updateGameOverMenu(G_gameOverMenu, gameContext);
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
      renderPlayState(G_playState, gameContext->renderer);
      break;
    }
    case MAIN_MENU_STATE: {
      renderMainMenu(G_mainMenu, gameContext->renderer);
      break;
    }
    case PAUSE_MENU_STATE: {
      renderPauseMenu(G_pauseMenu, gameContext->renderer);
      break;
    }
    case GAME_OVER_STATE: {
      renderGameOverMenu(G_gameOverMenu, gameContext->renderer);
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
      startGame(G_playState, gameContext);
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case EXIT_FROM_GAME: {
      gameContext->userInput.shouldQuit = true;
      break;
    }
    case MAIN_MENU: {
      gameContext->userInput = UserInput{};
      gameContext->currentState = MAIN_MENU_STATE;
      break;
    }
    case RESUME_PLAY: {
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case PAUSE_MENU: {
      gameContext->currentState = PAUSE_MENU_STATE;
      break;
    }
    case GAME_OVER: {
      gameContext->currentState = GAME_OVER_STATE;
      break;
    }
  }
}

extern "C" int
gameUpdateAndRender(GameContext *gameContext) {

  if (gameContext->currentState == NOT_INITIALIZED_STATE) {
    G_mainMenu = (MainMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(MainMenu));
    if (!initMainMenu(G_mainMenu, gameContext)) {
      return -1;
    }
    G_playState = (PlayState *) reserveMemory(&gameContext->permanentMemory, sizeof(PlayState));
    if (!initPlayState(G_playState, gameContext)) {
      return -1;
    }
    G_pauseMenu = (PauseMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(PauseMenu));
    if (!initPauseMenu(G_pauseMenu, gameContext)) {
      return -1;
    }
    G_gameOverMenu = (GameOverMenu *) reserveMemory(&gameContext->permanentMemory,
                                                    sizeof(GameOverMenu));
    if (!initGameOverMenu(G_gameOverMenu, gameContext)) {
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
