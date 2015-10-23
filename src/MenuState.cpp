//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "MenuState.h"
#include "TextureId.h"
#include "XmlUtils.h"

bool
MenuState::init(GameContext *gameContext) {

  if (!gameContext->functions.loadTexture("PLAY_BUTTON", "button.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("EXIT_BUTTON", "exit.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
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
  callbacks[0] = menuToPlay;
  callbacks[1] = exitFromGame;

  menuButtons[0] = (MenuButton *) reserveMemory(&gameContext->permanentMemory, sizeof(MenuButton));
  *menuButtons[0] = {100, 100, {textureArray[0], 400, 100, 3, 1, 1}, callbacks[0]};

  menuButtons[1] = (MenuButton *) reserveMemory(&gameContext->permanentMemory, sizeof(MenuButton));
  *menuButtons[1] = {100, 300, {textureArray[1], 400, 100, 3, 1, 1}, callbacks[1]};

  return true;
}

bool
MenuState::init(xmlDoc *doc, GameContext *gameContext) {
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
      TextureId textureId = getTextureId(id);
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
  callbacks[0] = menuToPlay;
  callbacks[1] = exitFromGame;

  xmlNode *objects = getXmlElement(menu, (const xmlChar *) "OBJECTS");
  if (!objects) {
    return false;
  }
  for (xmlNode *e = objects->children; e; e = e->next) {
    if (e->type == XML_ELEMENT_NODE && xmlStrcmp(e->name, (const xmlChar *) "object") == 0) {
      const xmlChar * type = xmlGetProp(e, (const xmlChar *) "type");
      if (xmlStrcmp(type, (const xmlChar *) "MenuButton") == 0) {
        char * id = (char *) xmlGetProp(e, (const xmlChar *) "id");
        int idx = atoi(id);
        char *x = (char *) xmlGetProp(e, (const xmlChar *) "x");
        char *y = (char *) xmlGetProp(e, (const xmlChar *) "y");
        char *textureId = (char *) xmlGetProp(e, (const xmlChar *) "textureId");
        char *width = (char *) xmlGetProp(e, (const xmlChar *) "width");
        char *height = (char *) xmlGetProp(e, (const xmlChar *) "height");
        char *totalFrames = (char *) xmlGetProp(e, (const xmlChar *) "totalFrames");
        char *currentFrame = (char *) xmlGetProp(e, (const xmlChar *) "currentFrame");
        char *currentRow = (char *) xmlGetProp(e, (const xmlChar *) "currentRow");
        menuButtons[idx] = (MenuButton *) reserveMemory(&gameContext->permanentMemory, sizeof(MenuButton));
        *menuButtons[idx] = {atoi(x), atoi(y),
                             {textureArray[idx], atoi(width), atoi(height), atoi(totalFrames),
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

void
MenuState::update(GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(menuButtons); ++i) {
    MenuButton *menuButton = menuButtons[i];
    menuButton->update(gameContext);
  }
}

void
MenuState::render(SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(menuButtons); ++i) {
    MenuButton *menuButton = menuButtons[i];
    menuButton->draw(renderer);
  }
}

GameStateId
MenuState::getStateId() const {
  return MENU_STATE;
}

void
MenuState::menuToPlay(GameContext *gameContext) {
  gameContext->stateChange = RESTART_PLAY;
}

void
MenuState::exitFromGame(GameContext *gameContext) {
  gameContext->stateChange = EXIT_FROM_GAME;
}
