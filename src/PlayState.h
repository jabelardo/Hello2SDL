//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

#include <SDL2/SDL_render.h>
#include <vector>
#include "GameState.h"
#include "InputHandler.h"
#include "TextureManager.h"

class Entity;
class Game;

class PlayState : public GameState {

public:
  virtual void update(InputHandler *pHandler);

  virtual void render(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onEnter(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onExit(TextureManager *textureManager);

  virtual int getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  std::vector<std::unique_ptr<Entity>> entities;
};


#endif //HELLO2SDL_PLAYSTATE_H
