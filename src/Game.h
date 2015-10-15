//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#include "GameObject.h"
#include <memory>
#include <vector>

class Game {
  TextureManager *textureManager = 0;
  SDL_Window *window = 0;
  SDL_Renderer *renderer = 0;
  bool running = true;
  std::vector<std::unique_ptr<GameObject>> gameObjects;

public:
  bool init(const char *title, int xpos, int ypos, int width, int height, int flags,
            TextureManager *textureManager);

  void handleEvents();

  void update();

  void render();

  void clean();

  bool isRunning();
};


#endif //HELLO2SDL_GAME_H
