//
// Created by Jose Gutierrez on 10/14/15.
//

#include "Game.h"

#define ANIMATE 1

bool
Game::init(const char *title, int xpos, int ypos, int width, int height,  int flags,
           TextureManager *textureManager) {
  this->textureManager = textureManager;
  if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

    if (window) {
      renderer = SDL_CreateRenderer(window, -1, 0);
    }

    if (renderer &&
        !textureManager->load(ANIMATE, "animate-alpha.png", renderer)) {
      return false;
    }
    gameObjects.push_back(std::make_unique<GameObject>());
    gameObjects.push_back(std::make_unique<GameObject>());
    gameObjects[0]->load(ANIMATE, 100, 100, 128, 82);
    gameObjects[1]->load(ANIMATE, 300, 300, 128, 82);
    return true;
  }
  return false;
}

void
Game::handleEvents() {
  auto event = SDL_Event{};
  if (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        running = false;
      } break;

      default:
        break;
    }
  }
}

void
Game::update() {
  for (auto& gameObject : gameObjects) {
    gameObject->update();
  }
}

void
Game::render() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (auto& gameObject : gameObjects) {
    gameObject->draw(textureManager, renderer);
  }

  SDL_RenderPresent(renderer);
}

void
Game::clean() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}

bool
Game::isRunning() {
  return running;
}