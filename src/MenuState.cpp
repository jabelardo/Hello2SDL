//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "MenuState.h"
#include "TextureId.h"

bool
MenuState::init(GameContext *gameContext) {

  if (!gameContext->functions.loadTexture(PLAY_BUTTON, "button.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(EXIT_BUTTON, "exit.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *textureArray[2] = {};

  textureArray[0] = gameContext->functions.getTexture(PLAY_BUTTON);
  if (!textureArray[0]) {
    return false;
  }
  textureArray[1] = gameContext->functions.getTexture(EXIT_BUTTON);
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
MenuState::init(tinyxml2::XMLDocument *xmlDoc, GameContext *gameContext) {
  auto states = xmlDoc->FirstChildElement("STATES");
  auto menu = states->FirstChildElement("MENU");
  auto textures = menu->FirstChildElement("TEXTURES");

  for (auto e = textures->FirstChildElement(); e; e = e->NextSiblingElement()) {
    auto id = e->Attribute("id");
    auto filename = e->Attribute("filename");
    auto textureId = getTextureId(id);
    if (!gameContext->functions.loadTexture(textureId, filename, gameContext->renderer)) {
      return false;
    }
  }

  SDL_Texture *textureArray[2] = {};

  textureArray[0] = gameContext->functions.getTexture(PLAY_BUTTON);
  if (!textureArray[0]) {
    return false;
  }
  textureArray[1] = gameContext->functions.getTexture(EXIT_BUTTON);
  if (!textureArray[1]) {
    return false;
  }

  GameContextCallbackFunc *callbacks[2] = {};
  callbacks[0] = menuToPlay;
  callbacks[1] = exitFromGame;

  auto objects = menu->FirstChildElement("OBJECTS");

  for (auto e = objects->FirstChildElement(); e; e = e->NextSiblingElement()) {
    auto type = e->Attribute("type");
    if (strcmp(type, "MenuButton") == 0) {
      auto idx = atoi(e->Attribute("id"));
      auto x = atoi(e->Attribute("x"));
      auto y = atoi(e->Attribute("y"));
      auto textureId = e->Attribute("textureId");
      auto width = atoi(e->Attribute("width"));
      auto height = atoi(e->Attribute("height"));
      auto totalFrames = atoi(e->Attribute("totalFrames"));
      auto currentFrame = atoi(e->Attribute("currentFrame"));
      auto currentRow = atoi(e->Attribute("currentRow"));

      menuButtons[idx] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
          MenuButton{x, y,
                     {textureArray[idx], width, height, totalFrames, currentFrame, currentRow},
                     callbacks[idx]};
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
