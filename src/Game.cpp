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

template<typename T>
bool
isNegative(T val) {
  int result = (val > T(0)) - (val < T(0));
  return result == -1;
}

void *
reserveMemory(MemoryPartition *partition, size_t reserveSize) {
  if (partition->type == PERMANENT_MEMORY || partition->type == SHORT_TIME_MEMORY) {
    assert(reserveSize <= partition->totalSize - partition->usedSize);
    int8_t *result = (int8_t *) partition->base + partition->usedSize;
    partition->usedSize += reserveSize;
    return result;

  } else if (partition->type == LONG_TIME_MEMORY) {
    void *block = partition->base;
    while (block < (int8_t *) partition->base + partition->totalSize) {
      // NOTE:
      // each memory block is prefixed with a ssize_t value indicating the block size, the block is
      // free to use if the sign of the size header is negative
      ssize_t blockSize = *(ssize_t *) block;
      if (isNegative(blockSize)) {
        if (-1 * blockSize >= reserveSize + sizeof(ssize_t)) {
          assert((int8_t *) block + sizeof(ssize_t) + reserveSize + sizeof(ssize_t)
                 <= (int8_t *) partition->base + partition->totalSize);
          ssize_t *resultAddress = (ssize_t *) block + 1;
          partition->usedSize += reserveSize + sizeof(ssize_t);
          *(ssize_t *) ((int8_t *) resultAddress + reserveSize) =
              blockSize + reserveSize + sizeof(ssize_t);
          *(ssize_t *) block = reserveSize;
          return resultAddress;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    assert(false);
    return 0;
  }
  assert(false);
  return 0;
}

bool
freeMemory(MemoryPartition *partition, void *memory) {
  if (partition->type == PERMANENT_MEMORY) {
    return false;

  } else if (partition->type == SHORT_TIME_MEMORY) {
    return true;

  } else if (partition->type == LONG_TIME_MEMORY) {
    assert(memory >= partition->base);
    assert(memory <= (int8_t *) partition->base + partition->totalSize);
    ssize_t *memorySize = (ssize_t *) memory - 1;

    if (isNegative(*memorySize)) {
      return false;
    }
    partition->usedSize -= *memorySize;
    *memorySize *= -1;

    // join contiguous free blocks
    void *block = partition->base;
    ssize_t *freeBlock = (ssize_t *) 0;
    while (block < (int8_t *) partition->base + partition->totalSize) {
      ssize_t blockSize = *(ssize_t *) block;
      if (isNegative(blockSize)) {
        if (freeBlock) {
          *freeBlock += blockSize - sizeof(ssize_t);
          partition->usedSize += blockSize - sizeof(ssize_t);
        } else {
          freeBlock = (ssize_t *) block;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        freeBlock = 0;
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    return true;
  }
  assert(false);
  return false;
}
