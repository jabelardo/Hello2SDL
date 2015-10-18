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

Game::Game(MenuState *menuState, PlayState *playState, PauseState *pauseState,
           GameOverState *gameOverState)
    : menuState(menuState), playState(playState), pauseState(pauseState),
      gameOverState(gameOverState) {
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
Game::restartPlay(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  playState->init(this, gameContext);
  stateMachine.changeState(playState);
}

void
Game::showMenu(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.clearStates();
  stateMachine.changeState(menuState);
}

void
Game::resumePlay(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.popState();
}

void
Game::pause(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.pushState(pauseState);
}

void
Game::gameOver(GameContext* gameContext) {
  *gameContext->userInput = UserInput{};
  stateMachine.changeState(gameOverState);
}

void
Game::quit() {
  shouldQuit = true;
}
