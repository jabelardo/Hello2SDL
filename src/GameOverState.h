//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_GAMEOVERSTATE_H
#define HELLO2SDL_GAMEOVERSTATE_H

#include <vector>
#include "GameState.h"
#include "MenuButton.h"
#include "AnimatedGraphic.h"

class Game;

class GameOverState : public GameState {

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
  std::unique_ptr<AnimatedGraphic> gameOverGraphic;

  static void gameOverToMain();

  static void restartPlay();
};


#endif //HELLO2SDL_GAMEOVERSTATE_H
