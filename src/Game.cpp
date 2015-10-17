//
// Created by Jose Gutierrez on 10/14/15.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Game.h"
#include "LoaderParams.h"
#include "Entity.h"
#include "UserInput.h"


Game::Game() {
  PlayState::setGame(this);
  MenuState::setGame(this);
  PauseState::setGame(this);
  GameOverState::setGame(this);
}

void
Game::update(UserInput *userInput, SDL_Renderer *renderer) {
  stateMachine.update(userInput, renderer);
  if (shouldQuit) {
    userInput->shouldQuit = true;
  }
}

void
Game::render(SDL_Renderer *renderer) {
  stateMachine.render(&textureManager, renderer);
}

void
Game::clean() {
}

void
Game::play(SDL_Renderer *renderer) {
  stateMachine.changeState(playState, &textureManager, renderer);
}

void
Game::showMenu(SDL_Renderer *renderer) {
  stateMachine.changeState(menuState, &textureManager, renderer);
}

void
Game::resumePlay() {
  stateMachine.popState(&textureManager);
}

void
Game::pause(SDL_Renderer *renderer) {
  stateMachine.pushState(pauseState, &textureManager, renderer);
}

void
Game::gameOver(SDL_Renderer *renderer) {
  stateMachine.changeState(gameOverState, &textureManager, renderer);
}

void
Game::quit() {
  shouldQuit = true;
}
