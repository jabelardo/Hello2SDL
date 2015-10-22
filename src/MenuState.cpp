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

  menuButtons[0] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton{100, 100, {textureArray[0], 400, 100, 3, 1, 1}, callbacks[0]};

  menuButtons[1] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton{100, 300, {textureArray[1], 400, 100, 3, 1, 1}, callbacks[1]};

  return true;
}

bool
MenuState::init(xmlDoc *doc, GameContext *gameContext) {
  auto root = xmlDocGetRootElement(doc);
  auto menu = getXmlElement(root, (const xmlChar *) "MENU");
  if (!menu) {
    return false;
  }
  auto textures = getXmlElement(menu, (const xmlChar *) "TEXTURES");
  if (!textures) {
    return false;
  }
  for (auto e = textures->children; e; e = e->next) {
    if (e->type == XML_ELEMENT_NODE && xmlStrcmp(e->name, (const xmlChar *) "texture") == 0) {
      auto id = (char *) xmlGetProp(e, (const xmlChar *) "id");
      auto filename = (char *) xmlGetProp(e, (const xmlChar *) "filename");
      auto textureId = getTextureId(id);
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

  auto objects = getXmlElement(menu, (const xmlChar *) "OBJECTS");
  if (!objects) {
    return false;
  }
  for (auto e = objects->children; e; e = e->next) {
    if (e->type == XML_ELEMENT_NODE && xmlStrcmp(e->name, (const xmlChar *) "object") == 0) {
      auto type = xmlGetProp(e, (const xmlChar *) "type");
      if (xmlStrcmp(type, (const xmlChar *) "MenuButton") == 0) {
        auto id = (char *) xmlGetProp(e, (const xmlChar *) "id");
        auto idx = atoi(id);
        auto x = (char *) xmlGetProp(e, (const xmlChar *) "x");
        auto y = (char *) xmlGetProp(e, (const xmlChar *) "y");
        auto textureId = (char *) xmlGetProp(e, (const xmlChar *) "textureId");
        auto width = (char *) xmlGetProp(e, (const xmlChar *) "width");
        auto height = (char *) xmlGetProp(e, (const xmlChar *) "height");
        auto totalFrames = (char *) xmlGetProp(e, (const xmlChar *) "totalFrames");
        auto currentFrame = (char *) xmlGetProp(e, (const xmlChar *) "currentFrame");
        auto currentRow = (char *) xmlGetProp(e, (const xmlChar *) "currentRow");
        menuButtons[idx] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
            MenuButton{atoi(x), atoi(y),
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
  for (auto &menuButton : menuButtons) {
    menuButton->update(gameContext);
  }
}

void
MenuState::render(SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
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
