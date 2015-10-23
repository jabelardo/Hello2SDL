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
#include "TileMap.cpp"
#include "MenuButton.cpp"
#include "MenuState.cpp"
#include "PauseState.cpp"
#include "Player.cpp"
#include "PlayState.cpp"
#include "RenderUtils.cpp"
#include "TextureId.cpp"

#include "XmlUtils.cpp"

extern "C" {
#include "tmx.c"
#include "tmx_err.c"
#include "tmx_utils.c"
#include "tmx_xml.c"
#include "tmx_sdl_test.c"
}

#include "GameContext.h"

Game *game;

extern "C" void
gameUpdateAndRender(GameContext *gameContext) {

  if (!gameContext->isInitialized) {

    MenuState *menuState = PLACEMENT_NEW(&gameContext->permanentMemory, MenuState) MenuState;

    PlayState *playState = PLACEMENT_NEW(&gameContext->permanentMemory, PlayState) PlayState;

    PauseState *pauseState = PLACEMENT_NEW(&gameContext->permanentMemory, PauseState) PauseState;

    GameOverState *gameOverState = PLACEMENT_NEW(&gameContext->permanentMemory, GameOverState) GameOverState;

    game = (Game *) reserveMemory(&gameContext->permanentMemory, sizeof(Game));
    *game = {menuState, playState, pauseState, gameOverState};

//    doc = xmlReadFile("/Users/jabelardo/Library/Caches/clion11/cmake/generated/cd64e9d/cd64e9d/Debug/assets/test.xml", 0, 0);
//    menuState->init(doc, gameContext);
//    xmlFreeDoc(doc);

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