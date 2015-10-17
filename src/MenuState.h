//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUSTATE_H
#define HELLO2SDL_MENUSTATE_H

#include <vector>
#include "GameState.h"
#include "MenuButton.h"
#include "UserInput.h"

class Game;

class MenuState : public GameState {

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

  static void menuToPlay(GameContext* gameContext);
  static void exitFromMenu(GameContext* gameContext);
};


#endif //HELLO2SDL_MENUSTATE_H
