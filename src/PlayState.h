//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

#include <vector>
#include "GameState.h"
#include "UserInput.h"
#include "Player.h"
#include "Entity.h"

class Game;

class PlayState : public GameState {

public:
  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual bool onEnter(GameContext* gameContext);

  virtual bool onExit(GameContext* gameContext);

  virtual GameStateId getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  Player player;
  std::vector<Entity> entities;
};


#endif //HELLO2SDL_PLAYSTATE_H
