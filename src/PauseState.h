//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_PAUSESTATE_H
#define HELLO2SDL_PAUSESTATE_H

#include <vector>
#include "GameState.h"
#include "MenuButton.h"

class Game;

class PauseState : public GameState {

public:
  virtual void update(InputHandler *inputHandler);

  virtual void render(TextureManager *textureManager,
                      SDL_Renderer *renderer);

  virtual bool onEnter(TextureManager *textureManager,
                       SDL_Renderer *renderer);

  virtual bool onExit(TextureManager *textureManager);

  virtual GameStateId getStateId() const;

  static void setGame(Game *game);

private:
  static Game* game;
  std::vector<MenuButton> menuButtons;

  static void pauseToMain();
  static void resumePlay();
};


#endif //HELLO2SDL_PAUSESTATE_H
