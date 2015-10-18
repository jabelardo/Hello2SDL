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
#include "RenderUtils.cpp"
#include "GameContext.h"

Game *game;

extern "C" void
gameUpdateAndRender(GameContext *gameContext) {

  if (!gameContext->isInitialized) {

    auto menuState = PLACEMENT_NEW(&gameContext->permanentMemory, MenuState)
        MenuState;

    auto playState = PLACEMENT_NEW(&gameContext->permanentMemory, PlayState)
        PlayState;

    auto pauseState = PLACEMENT_NEW(&gameContext->permanentMemory, PauseState)
        PauseState;

    auto gameOverState = PLACEMENT_NEW(&gameContext->permanentMemory, GameOverState)
        GameOverState;

    game = PLACEMENT_NEW(&gameContext->permanentMemory, Game)
    Game(menuState, playState, pauseState, gameOverState);

    menuState->init(game, gameContext);
    playState->init(game, gameContext);
    pauseState->init(game, gameContext);
    gameOverState->init(game, gameContext);

    game->showMenu(gameContext);
    gameContext->isInitialized = true;
  }
  game->update(gameContext);
  game->render(gameContext->renderer);
}