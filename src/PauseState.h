//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_PAUSESTATE_H
#define HELLO2SDL_PAUSESTATE_H

#include <vector>
#include "GameState.h"
#include "MenuButton.h"
#include "UserInput.h"

class Game;

class PauseState : public GameState {

public:
  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual bool onEnter(GameContext* gameContext);

  virtual bool onExit(GameContext* gameContext);

  virtual GameStateId getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  std::vector<MenuButton> menuButtons;

  static void pauseToMain(GameContext* gameContext);
  static void resumePlay(GameContext* gameContext);
};


#endif //HELLO2SDL_PAUSESTATE_H
