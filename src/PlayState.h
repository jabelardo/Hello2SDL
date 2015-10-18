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
  bool init(Game* game, GameContext* gameContext);

  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual GameStateId getStateId() const;

private:
  static Game* game;
  Player* player;
  Entity* enemy;
};


#endif //HELLO2SDL_PLAYSTATE_H
