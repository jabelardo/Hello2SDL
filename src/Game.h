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

class Game {
public:

  Game(MenuState *menuState, PlayState *playState, PauseState *pauseState,
       GameOverState *gameOverState);

  void update(GameContext* gameContext);

  void render(SDL_Renderer* renderer);

  void restartPlay(GameContext* gameContext);

  void showMenu(GameContext* gameContext);

  void resumePlay(GameContext* gameContext);

  void pause(GameContext* gameContext);

  void gameOver(GameContext* gameContext);

  void quit();

private:
  MenuState* menuState;
  PlayState* playState;
  PauseState* pauseState;
  GameOverState* gameOverState;
  GameStateMachine stateMachine;
  bool shouldQuit = false;
};

#endif //HELLO2SDL_GAME_H
