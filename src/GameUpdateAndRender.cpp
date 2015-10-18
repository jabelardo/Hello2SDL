//
// Created by Jose Gutierrez on 10/16/15.
//

#include "GameUpdateAndRender.h"

#include "AnimatedGraphic.cpp"
#include "Enemy.cpp"
#include "Entity.cpp"
#include "Game.cpp"
#include "GameOverState.cpp"
#include "GameStateMachine.cpp"
#include "MenuButton.cpp"
#include "MenuState.cpp"
#include "PauseState.cpp"
#include "Player.cpp"
#include "PlayState.cpp"
#include "RenderUtils.cpp"
#include "TextureId.cpp"
#include "tinyxml2.cpp"
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

//    tinyxml2::XMLDocument xmlDoc;
//    auto xmlErr = xmlDoc.LoadFile("/Users/jabelardo/Library/Caches/clion11/cmake/generated/cd64e9d/cd64e9d/Debug/assets/test.xml");
//    menuState->init(&xmlDoc, gameContext);
    menuState->init(gameContext);
    playState->init(gameContext);
    pauseState->init(gameContext);
    gameOverState->init(gameContext);

    game->showMainMenu(gameContext);
    gameContext->isInitialized = true;
  }
  game->update(gameContext);
  game->render(gameContext->renderer);
}