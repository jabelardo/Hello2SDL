//
// Created by Jose Gutierrez on 10/14/15.
//

#include "Game.h"

#define ANIMATE_ALPHA_ID 1

bool
Game::init(const char *title, int xpos, int ypos, int width, int height,
           int flags, TextureManager *textureManager) {
  this->textureManager = textureManager;
  if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

    if (window) {
      renderer = SDL_CreateRenderer(window, -1, 0);
    }

    if (renderer) {
      return textureManager->load(ANIMATE_ALPHA_ID, "animate-alpha.png", renderer);
    }
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
  currentFrame = int(((SDL_GetTicks() / 100) % 6));
}

void
Game::render() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  textureManager->draw(ANIMATE_ALPHA_ID, 0, 0, 128, 82, renderer);

  textureManager->drawFrame(ANIMATE_ALPHA_ID, 100, 100, 128, 82, 1, currentFrame, renderer);

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