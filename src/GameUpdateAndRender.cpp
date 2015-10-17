//
// Created by Jose Gutierrez on 10/16/15.
//

#include "GameUpdateAndRender.h"

#include "AnimatedGraphic.cpp"
#include "Entity.cpp"
#include "Game.cpp"
#include "GameOverState.cpp"
#include "GameStateMachine.cpp"
#include "MenuButton.cpp"
#include "PauseState.cpp"
#include "Player.cpp"
#include "PlayState.cpp"
#include "MenuState.cpp"
#include "Sprite.cpp"
#include "TextureManager.cpp"
#include "GameContext.h"

std::unique_ptr<Game> game;

extern "C" void
gameUpdateAndRender(GameContext *gameContext, UserInput *userInput, SDL_Renderer *renderer) {

  if (!gameContext->isInitialized) {
    game = std::make_unique<Game>();
    game->showMenu(renderer);
    gameContext->isInitialized = true;
  }
  game->update(userInput, renderer);
  game->render(renderer);
}