//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_PAUSESTATE_H
#define HELLO2SDL_PAUSESTATE_H

#include "GameState.h"
#include "MenuButton.h"
#include "UserInput.h"

class Game;

class PauseState : public GameState {

public:
  bool init(Game* game, GameContext* gameContext);

  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual GameStateId getStateId() const;

private:
  static Game* game;
  MenuButton* menuButtons[2];

  static void pauseToMain(GameContext* gameContext);
  static void resumePlay(GameContext* gameContext);
};


#endif //HELLO2SDL_PAUSESTATE_H
