//
// Created by Jose Gutierrez on 10/14/15.
//

#ifdef __APPLE__

#include <SDL2/SDL.h>

#else
#include <SDL.h>
#endif

#include "Game.h"
#include "Entity.h"
#include "UserInput.h"

void
Game::update(GameContext *gameContext) {
  gameContext->stateChange = NONE;
  stateMachine.update(gameContext);
  processStateChange(gameContext);
}

void
Game::processStateChange(GameContext *gameContext) {
  switch (gameContext->stateChange) {
    case NONE: {
      break;
    }
    case RESTART_PLAY: {
      restartPlay(gameContext);
      break;
    }
    case EXIT_FROM_GAME: {
      gameContext->userInput->shouldQuit = true;
      break;
    }
    case SHOW_MAIN_MENU: {
      showMainMenu(gameContext);
      break;
    }
    case RESUME_PLAY: {
      resumePlay(gameContext);
      break;
    }
    case PAUSE_PLAY: {
      pausePlay(gameContext);
      break;
    }
    case GAME_OVER: {
      gameOver(gameContext);
      break;
    }
  }
}

void
Game::render(SDL_Renderer *renderer) {
  stateMachine.render(renderer);
}

void
Game::restartPlay(GameContext *gameContext) {
  *gameContext->userInput = UserInput{};
  playState->init(gameContext);
  stateMachine.changeState(playState);
}

void
Game::showMainMenu(GameContext *gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.clearStates();
  stateMachine.changeState(menuState);
}

void
Game::resumePlay(GameContext *gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.popState();
}

void
Game::pausePlay(GameContext *gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.pushState(pauseState);
}

void
Game::gameOver(GameContext *gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.changeState(gameOverState);
}
