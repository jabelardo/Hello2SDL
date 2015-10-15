//
// Created by Jose Gutierrez on 10/14/15.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Game.h"

#define ANIMATE 1

bool
Game::init(const char *title, int xpos, int ypos, int width, int height, Uint32 flags,
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
    entities.push_back(std::make_unique<Entity>(Entity::PlayerType,
                                                LoaderParams{ANIMATE, 100, 100, 128, 82}));

    entities.push_back(std::make_unique<Entity>(Entity::DefaultType,
                                                LoaderParams{ANIMATE, 300, 300, 128, 82}));

    entities.push_back(std::make_unique<Entity>(Entity::EnemyType,
                                                LoaderParams{ANIMATE, 0, 0, 128, 82}));

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
      }
        break;

      default:
        break;
    }
  }
}

void
Game::update() {
  for (auto &Entity : entities) {
    switch (Entity->getType()) {
      case Entity::PlayerType: {
        Entity->setAcceleration(Vector2D{.025,0});
        Entity->setVelocity(Entity->getVelocity() + Entity->getAcceleration());
        Entity->setPosition(Entity->getPosition() + Entity->getVelocity());
      }
        break;
      case Entity::DefaultType: {
        Entity->setPosition(Entity->getPosition() - Vector2D{1,0});
      }
        break;
      case Entity::EnemyType: {
        Entity->setPosition(Entity->getPosition() + Vector2D{1,1});
      }
        break;
    }
    Entity->frameUpdate();
  }
}

void
Game::render() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (auto &Entity : entities) {
    Entity->draw(textureManager, renderer);
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