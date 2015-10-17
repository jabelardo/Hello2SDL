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
#include "TextureManager.h"
#include "UserInput.h"

class Game {
public:
  Game();

  void update(UserInput *userInput, SDL_Renderer *renderer);

  void render(SDL_Renderer *renderer);

  void play(SDL_Renderer *renderer);

  void showMenu(SDL_Renderer *renderer);

  void resumePlay();

  void pause(SDL_Renderer *renderer);

  void gameOver(SDL_Renderer *renderer);

  void quit();

private:
  TextureManager textureManager;
  GameStateMachine stateMachine;
  PlayState playState;
  MenuState menuState;
  PauseState pauseState;
  GameOverState gameOverState;
  bool shouldQuit = false;
};

#endif //HELLO2SDL_GAME_H
