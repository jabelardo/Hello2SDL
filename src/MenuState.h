//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUSTATE_H
#define HELLO2SDL_MENUSTATE_H

#include "GameState.h"
#include "MenuButton.h"
#include "UserInput.h"

class Game;

class MenuState : public GameState {

public:
  bool init(Game* game, GameContext* gameContext);

  virtual void update(GameContext* gameContext);

  virtual void render(SDL_Renderer* renderer);

  virtual GameStateId getStateId() const;

private:
  static Game* game;
  MenuButton* menuButtons[2];

  static void menuToPlay(GameContext* gameContext);
  static void exitFromMenu(GameContext* gameContext);
};


#endif //HELLO2SDL_MENUSTATE_H
