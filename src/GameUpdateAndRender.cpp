//
// Created by Jose Gutierrez on 10/16/15.
//

#include "GameUpdateAndRender.h"
#include "Entity.h"

#include "AnimatedGraphic.cpp"
#include "TileMap.cpp"
#include "MenuButton.cpp"
#include "RenderUtils.cpp"

#include "XmlUtils.cpp"

extern "C" {
#include "tmx.c"
#include "tmx_err.c"
#include "tmx_utils.c"
#include "tmx_xml.c"
#include "tmx_sdl_test.c"
}

#include "GameContext.h"

struct MainMenu {
  union {
    struct {
      MenuButton *startPlayButton;
      MenuButton *exitButton;
    };
    MenuButton *menuButtons[2];
  };
};

struct PlayState {
  TileMap *tileMap;
  Entity *enemy;
};

struct PauseMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *resumePlayButton;
    };
    MenuButton *menuButtons[2];
  };
};

struct GameOverMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *startPlayButton;
    };
    MenuButton *menuButtons[2];
  };
  AnimatedGraphic *gameOverGraphic;
};



MainMenu *G_mainMenu;

PlayState *G_playState;

PauseMenu *G_pauseMenu;

GameOverMenu *G_gameOverMenu;

void
startPlay(GameContext *gameContext) {
  gameContext->stateChange = START_PLAY;
}

void
exitFromGame(GameContext *gameContext) {
  gameContext->stateChange = EXIT_FROM_GAME;
}

bool
initMainMenu(MainMenu *mainMenu, xmlDoc *doc, GameContext *gameContext) {
  xmlNode *root = xmlDocGetRootElement(doc);
  xmlNode *menu = getXmlElement(root, (const xmlChar *) "MENU");
  if (!menu) {
    return false;
  }
  xmlNode *textures = getXmlElement(menu, (const xmlChar *) "TEXTURES");
  if (!textures) {
    return false;
  }
  for (xmlNode *e = textures->children; e; e = e->next) {
    if (e->type == XML_ELEMENT_NODE && xmlStrcmp(e->name, (const xmlChar *) "texture") == 0) {
      char *id = (char *) xmlGetProp(e, (const xmlChar *) "id");
      char *filename = (char *) xmlGetProp(e, (const xmlChar *) "filename");
      if (!gameContext->functions.loadTexture("textureId", filename, gameContext->renderer,
                                              &gameContext->permanentMemory)) {
        xmlFree(id);
        xmlFree(filename);
        return false;
      }
      xmlFree(id);
      xmlFree(filename);
    }
  }

  SDL_Texture *textureArray[2] = {};

  textureArray[0] = gameContext->functions.getTexture("PLAY_BUTTON");
  if (!textureArray[0]) {
    return false;
  }
  textureArray[1] = gameContext->functions.getTexture("EXIT_BUTTON");
  if (!textureArray[1]) {
    return false;
  }

  GameContextCallbackFunc *callbacks[2] = {};
  callbacks[0] = startPlay;
  callbacks[1] = exitFromGame;

  xmlNode *objects = getXmlElement(menu, (const xmlChar *) "OBJECTS");
  if (!objects) {
    return false;
  }
  for (xmlNode *e = objects->children; e; e = e->next) {
    if (e->type == XML_ELEMENT_NODE && xmlStrcmp(e->name, (const xmlChar *) "object") == 0) {
      const xmlChar *type = xmlGetProp(e, (const xmlChar *) "type");
      if (xmlStrcmp(type, (const xmlChar *) "MenuButton") == 0) {
        char *id = (char *) xmlGetProp(e, (const xmlChar *) "id");
        int idx = atoi(id);
        char *x = (char *) xmlGetProp(e, (const xmlChar *) "x");
        char *y = (char *) xmlGetProp(e, (const xmlChar *) "y");
        char *textureId = (char *) xmlGetProp(e, (const xmlChar *) "textureId");
        char *width = (char *) xmlGetProp(e, (const xmlChar *) "width");
        char *height = (char *) xmlGetProp(e, (const xmlChar *) "height");
        char *totalFrames = (char *) xmlGetProp(e, (const xmlChar *) "totalFrames");
        char *currentFrame = (char *) xmlGetProp(e, (const xmlChar *) "currentFrame");
        char *currentRow = (char *) xmlGetProp(e, (const xmlChar *) "currentRow");
        mainMenu->menuButtons[idx] = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                                  sizeof(MenuButton));
        *mainMenu->menuButtons[idx] = {atoi(x), atoi(y),
                                       {textureArray[idx], atoi(width), atoi(height),
                                        atoi(totalFrames),
                                        atoi(currentFrame), atoi(currentRow)},
                                       callbacks[idx]};
        xmlFree(id);
        xmlFree(x);
        xmlFree(y);
        xmlFree(textureId);
        xmlFree(width);
        xmlFree(height);
        xmlFree(totalFrames);
        xmlFree(currentFrame);
        xmlFree(currentRow);
      }
    }
  }
  return false;
}

bool
initMainMenu(MainMenu *mainMenu, GameContext *gameContext) {

  if (!gameContext->functions.loadTexture("PLAY_BUTTON", "button.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("EXIT_BUTTON", "exit.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }

  SDL_Texture *playButtonTexture = gameContext->functions.getTexture("PLAY_BUTTON");
  if (!playButtonTexture) {
    return false;
  }
  SDL_Texture *exitButtonTexture = gameContext->functions.getTexture("EXIT_BUTTON");
  if (!exitButtonTexture) {
    return false;
  }

  mainMenu->startPlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                           sizeof(MenuButton));
  *mainMenu->startPlayButton = {100, 100, {playButtonTexture, 400, 100, 3, 1, 1}, startPlay};

  mainMenu->exitButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                      sizeof(MenuButton));
  *mainMenu->exitButton = {100, 300, {exitButtonTexture, 400, 100, 3, 1, 1}, exitFromGame};

  return true;
}

void
updateMainMenu(MainMenu *mainMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
  }
}

void
renderMainMenu(MainMenu *mainMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}

bool
initPlayState(PlayState *playState, GameContext *gameContext) {
  if (!gameContext->functions.loadTexture("HELICOPTER2", "helicopter2.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }

  playState->tileMap = (TileMap *) reserveMemory(&gameContext->permanentMemory, sizeof(TileMap));

  if (!initTileMap(playState->tileMap,
                   gameContext,
                   "/Users/jabelardo/Library/Caches/clion11/cmake/generated/cd64e9d/cd64e9d/Debug/assets/untitled2.tmx")) {
    return false;
  }

  playState->enemy = (Entity *) reserveMemory(&gameContext->permanentMemory, sizeof(Entity));

  return true;
}

bool
startGame(PlayState *playState, GameContext *gameContext) {
  SDL_Texture *helicopter2 = gameContext->functions.getTexture("HELICOPTER2");
  if (!helicopter2) {
    return false;
  }
  for (TileLayer* tileLayer = playState->tileMap->tileLayerList; tileLayer; tileLayer = tileLayer->next) {
    tileLayer->position = {0, 0};
  }

  playState->tileMap->objectLayer->player->position =
      playState->tileMap->objectLayer->playerInitialPosition;

  *playState->enemy = {ENEMY_TYPE, {400, 100}, {helicopter2, 128, 55, 5, 1, 1}, {2, .33f}, {0, .33f}};
  return true;
}

bool
checkEntityCollision(Entity *entity1, Entity *entity2) {
  float leftA = entity1->position.x;
  float rightA = entity1->position.x + entity1->bitmap.width;
  float topA = entity1->position.y;
  float bottomA = entity1->position.y + entity1->bitmap.height;
  //Calculate the sides of rect B
  float leftB = entity2->position.x;
  float rightB = entity2->position.x + entity2->bitmap.width;
  float topB = entity2->position.y;
  float bottomB = entity2->position.y + entity2->bitmap.height;
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}


void
drawEntity(Entity *entity, SDL_Renderer *renderer) {
  switch (entity->type) {
    case ENEMY_TYPE:
    case PLAYER_TYPE: {
      if (entity->velocity.x > 0) {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                   SDL_FLIP_HORIZONTAL);
      } else {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);
      }
    }
  }
}

void
updateEntity(Entity *entity, UserInput *userInput) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      entity->velocity = V2D{0, 0};

      V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
      entity->velocity = (target - entity->position) / 50;

      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
    }
      break;

    case ENEMY_TYPE: {
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      if (entity->position.y < 0) {
        entity->velocity = V2D{2, .33f};
        entity->acceleration = V2D{0, .33f};

      } else if (entity->position.y > 400) {
        entity->velocity = V2D{2, -.33f};
        entity->acceleration = V2D{0, -.33f};
      }
      if (entity->position.x > 640) {
        entity->position.x = 0;
      }
      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
    }
      break;
  }
}

void
updatePlayState(PlayState *playState, GameContext *gameContext) {
  if (gameContext->userInput->back.endedDown) {
    gameContext->stateChange = PAUSE_MENU;
    return;
  }
  updateTileMap(playState->tileMap, gameContext);
  updateEntity(playState->enemy, gameContext->userInput);

  if (checkEntityCollision(playState->enemy, playState->tileMap->objectLayer->player)) {
    gameContext->stateChange = GAME_OVER;
  }
}

void
renderPlayState(PlayState *playState, SDL_Renderer *renderer) {
  drawTileMap(playState->tileMap, renderer);
  drawEntity(playState->enemy, renderer);
}

void
resumePlay(GameContext *gameContext) {
  gameContext->stateChange = RESUME_PLAY;
}

void
mainMenu(GameContext *gameContext) {
  gameContext->stateChange = MAIN_MENU;
}

bool
initPauseMenu(PauseMenu *pauseMenu, GameContext *gameContext) {
  if (!gameContext->functions.loadTexture("MAIN_BUTTON", "main.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("RESUME_BUTTON", "resume.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }

  SDL_Texture *mainButton = gameContext->functions.getTexture("MAIN_BUTTON");
  if (!mainButton) {
    return false;
  }
  SDL_Texture *resumeButton = gameContext->functions.getTexture("RESUME_BUTTON");
  if (!resumeButton) {
    return false;
  }
  pauseMenu->mainMenuButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                           sizeof(MenuButton));
  *pauseMenu->mainMenuButton = {200, 100, {mainButton, 200, 80, 3, 1, 1}, mainMenu};

  pauseMenu->resumePlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                             sizeof(MenuButton));
  *pauseMenu->resumePlayButton = {200, 300, {resumeButton, 200, 80, 3, 1, 1}, resumePlay};

  return true;
}

void
updatePauseMenu(PauseMenu *pauseMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
  }
}

void
renderPauseMenu(PauseMenu *pauseMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}

bool
initGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext) {

  if (!gameContext->functions.loadTexture("GAME_OVER_TEXT", "gameover.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("MAIN_BUTTON", "main.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("RESTART_BUTTON", "restart.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  SDL_Texture *mainButton = gameContext->functions.getTexture("MAIN_BUTTON");
  if (!mainButton) {
    return false;
  }
  SDL_Texture *restartButton = gameContext->functions.getTexture("RESTART_BUTTON");
  if (!restartButton) {
    return false;
  }
  SDL_Texture *gameOverText = gameContext->functions.getTexture("GAME_OVER_TEXT");
  if (!gameOverText) {
    return false;
  }
  gameOverMenu->mainMenuButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                              sizeof(MenuButton));
  *gameOverMenu->mainMenuButton = {200, 200, {mainButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                   mainMenu};

  gameOverMenu->startPlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                               sizeof(MenuButton));

  *gameOverMenu->startPlayButton = {200, 300, {restartButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                    startPlay};

  gameOverMenu->gameOverGraphic = (AnimatedGraphic *) reserveMemory(&gameContext->permanentMemory,
                                                                    sizeof(AnimatedGraphic));
  *gameOverMenu->gameOverGraphic = {200, 100, {gameOverText, 190, 30, 2, 1, 1}, 2};

  return true;
}

void
updateGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(gameOverMenu->menuButtons); ++i) {
    MenuButton *menuButton = gameOverMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
  }
  updateAnimatedGraphic(gameOverMenu->gameOverGraphic);
}

void
renderGameOverMenu(GameOverMenu *gameOverMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(gameOverMenu->menuButtons); ++i) {
    MenuButton *menuButton = gameOverMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
  drawAnimatedGraphic(gameOverMenu->gameOverGraphic, renderer);
}

void
updateGame(GameContext *gameContext) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      updatePlayState(G_playState, gameContext);
      break;
    }
    case MAIN_MENU_STATE: {
      updateMainMenu(G_mainMenu, gameContext);
      break;
    }
    case PAUSE_MENU_STATE: {
      updatePauseMenu(G_pauseMenu, gameContext);
      break;
    }
    case GAME_OVER_STATE: {
      updateGameOverMenu(G_gameOverMenu, gameContext);
      break;
    }
  }
}

void
renderGame(GameContext *gameContext) {
  switch (gameContext->currentState) {
    case PLAY_STATE: {
      renderPlayState(G_playState, gameContext->renderer);
      break;
    }
    case MAIN_MENU_STATE: {
      renderMainMenu(G_mainMenu, gameContext->renderer);
      break;
    }
    case PAUSE_MENU_STATE: {
      renderPauseMenu(G_pauseMenu, gameContext->renderer);
      break;
    }
    case GAME_OVER_STATE: {
      renderGameOverMenu(G_gameOverMenu, gameContext->renderer);
      break;
    }
  }
}

void
processStateChange(GameContext *gameContext) {
  switch (gameContext->stateChange) {
    case NONE: {
      break;
    }
    case START_PLAY: {
      startGame(G_playState, gameContext);
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case EXIT_FROM_GAME: {
      gameContext->userInput->shouldQuit = true;
      break;
    }
    case MAIN_MENU: {
      *gameContext->userInput = UserInput{};
      gameContext->currentState = MAIN_MENU_STATE;
      break;
    }
    case RESUME_PLAY: {
      gameContext->currentState = PLAY_STATE;
      break;
    }
    case PAUSE_MENU: {
      gameContext->currentState = PAUSE_MENU_STATE;
      break;
    }
    case GAME_OVER: {
      gameContext->currentState = GAME_OVER_STATE;
      break;
    }
  }
}

extern "C" int
gameUpdateAndRender(GameContext *gameContext) {

  if (!gameContext->isInitialized) {
    G_mainMenu = (MainMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(MainMenu));
    if (!initMainMenu(G_mainMenu, gameContext)) {
      return -1;
    }
    G_playState = (PlayState *) reserveMemory(&gameContext->permanentMemory, sizeof(PlayState));
    if (!initPlayState(G_playState, gameContext)) {
      return -1;
    }
    G_pauseMenu = (PauseMenu *) reserveMemory(&gameContext->permanentMemory, sizeof(PauseMenu));
    if (!initPauseMenu(G_pauseMenu, gameContext)) {
      return -1;
    }
    G_gameOverMenu = (GameOverMenu *) reserveMemory(&gameContext->permanentMemory,
                                                    sizeof(GameOverMenu));
    if (!initGameOverMenu(G_gameOverMenu, gameContext)) {
      return -1;
    }
    gameContext->currentState = MAIN_MENU_STATE;
    gameContext->isInitialized = true;
  }
  gameContext->stateChange = NONE;
  updateGame(gameContext);
  processStateChange(gameContext);
  renderGame(gameContext);

  return 0;
}
