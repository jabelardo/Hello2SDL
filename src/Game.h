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
  Game();

  void update(GameContext* gameContext);

  void render(SDL_Renderer* renderer);

  void play(GameContext* gameContext);

  void showMenu(GameContext* gameContext);

  void resumePlay(GameContext* gameContext);

  void pause(GameContext* gameContext);

  void gameOver(GameContext* gameContext);

  void quit();

private:
  GameStateMachine stateMachine;
  PlayState playState;
  MenuState menuState;
  PauseState pauseState;
  GameOverState gameOverState;
  bool shouldQuit = false;
};

#endif //HELLO2SDL_GAME_H
