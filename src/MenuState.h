//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUSTATE_H
#define HELLO2SDL_MENUSTATE_H

#include <libxml/tree.h>

#include "GameState.h"
#include "MenuButton.h"
#include "UserInput.h"

class MenuState : public GameState {

public:
  bool init(xmlDoc *doc, GameContext *gameContext) ;

  virtual bool init(GameContext *gameContext);

  virtual void update(GameContext *gameContext);

  virtual void render(SDL_Renderer *renderer);

  virtual GameStateId getStateId() const;

private:
  MenuButton *menuButtons[2];

  static void menuToPlay(GameContext *gameContext);

  static void exitFromGame(GameContext *gameContext);
};


#endif //HELLO2SDL_MENUSTATE_H
