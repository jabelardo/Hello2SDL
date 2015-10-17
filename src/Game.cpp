//
// Created by Jose Gutierrez on 10/14/15.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Game.h"
#include "Entity.h"
#include "UserInput.h"


Game::Game() {
  PlayState::setGame(this);
  MenuState::setGame(this);
  PauseState::setGame(this);
  GameOverState::setGame(this);
}

void
Game::update(GameContext* gameContext) {
  stateMachine.update(gameContext);
  if (shouldQuit) {
    gameContext->userInput->shouldQuit = true;
  }
}

void
Game::render(SDL_Renderer* renderer) {
  stateMachine.render(renderer);
}

void
Game::play(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.changeState(&playState, gameContext);
}

void
Game::showMenu(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.clear(gameContext);
  stateMachine.changeState(&menuState, gameContext);
}

void
Game::resumePlay(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.popState(gameContext);
}

void
Game::pause(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.pushState(&pauseState, gameContext);
}

void
Game::gameOver(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.changeState(&gameOverState, gameContext);
}

void
Game::quit() {
  shouldQuit = true;
}
