//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#include <memory>
#include <vector>

#include "Entity.h"
#include "InputHandler.h"
#include "GameStateMachine.h"

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

private:
  TextureManager *textureManager = 0;
  SDL_Window *window = 0;
  SDL_Renderer *renderer = 0;
  bool running = true;
  //std::vector<std::unique_ptr<Entity>> entities;
  InputHandler inputHandler = InputHandler{};
  GameStateMachine stateMachine;
};


#endif //HELLO2SDL_GAME_H
