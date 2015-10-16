//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

#include <vector>
#include "GameState.h"

class Entity;
class Game;
class Sprite;

class PlayState : public GameState {

public:
  virtual void update(InputHandler *pHandler);

  virtual void render(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onEnter(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onExit(TextureManager *textureManager);

  virtual GameStateId getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  std::vector<std::unique_ptr<Entity>> entities;
};


#endif //HELLO2SDL_PLAYSTATE_H
