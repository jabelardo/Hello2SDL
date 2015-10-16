//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUSTATE_H
#define HELLO2SDL_MENUSTATE_H

#include <vector>
#include "GameState.h"
#include "MenuButton.h"

class Game;

class MenuState : public GameState {

public:
  virtual void update(InputHandler *inputHandler);

  virtual void render(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onEnter(TextureManager *textureManager, SDL_Renderer *renderer);

  virtual bool onExit(TextureManager *textureManager);

  virtual GameStateId getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  std::vector<MenuButton> menuButtons;

  static void menuToPlay();
  static void exitFromMenu();
};


#endif //HELLO2SDL_MENUSTATE_H
