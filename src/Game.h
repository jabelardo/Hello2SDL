//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#include "GameStateMachine.h"
#include "MenuState.h"
#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "RenderUtils.h"
#include "UserInput.h"

struct Game {

  void update(GameContext *gameContext);

  void render(SDL_Renderer *renderer);

  void restartPlay(GameContext *gameContext);

  void showMainMenu(GameContext *gameContext);

  void resumePlay(GameContext *gameContext);

  void pausePlay(GameContext *gameContext);

  void gameOver(GameContext *gameContext);

  void processStateChange(GameContext *gameContext);

  MenuState *menuState;
  PlayState *playState;
  PauseState *pauseState;
  GameOverState *gameOverState;
  GameStateMachine stateMachine;
};

#endif //HELLO2SDL_GAME_H
