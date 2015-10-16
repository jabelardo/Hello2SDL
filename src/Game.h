//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#include "Entity.h"
#include "InputHandler.h"
#include "GameStateMachine.h"
#include "PlayState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "GameOverState.h"

class TextureManager;

class Game {

public:
  bool init(const char *title, int xpos, int ypos, int width, int height, Uint32 flags,
            TextureManager *textureManager);

  void handleEvents();

  void update();

  void render();

  void clean();

  bool isRunning();

  void quit();

  void play();

  void showMenu();

  void resumePlay();

  void pause();

  void resetInput();

  void gameOver();

private:
  TextureManager *textureManager = 0;
  SDL_Window *window = 0;
  SDL_Renderer *renderer = 0;
  bool running = true;
  InputHandler inputHandler = InputHandler{};
  GameStateMachine stateMachine;
  PlayState playState;
  MenuState menuState;
  PauseState pauseState;
  GameOverState gameOverState;
};


#endif //HELLO2SDL_GAME_H
