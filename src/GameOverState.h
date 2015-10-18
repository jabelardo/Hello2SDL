//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_GAMEOVERSTATE_H
#define HELLO2SDL_GAMEOVERSTATE_H

#include "GameState.h"
#include "MenuButton.h"
#include "AnimatedGraphic.h"
#include "UserInput.h"

class Game;

class GameOverState : public GameState {

public:
  bool init(Game* game, GameContext* gameContext);

  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual GameStateId getStateId() const;

private:
  static Game* game;
  MenuButton* menuButtons[2];
  AnimatedGraphic* gameOverGraphic;

  static void gameOverToMain(GameContext* gameContext);

  static void restartPlay(GameContext* gameContext);
};


#endif //HELLO2SDL_GAMEOVERSTATE_H
