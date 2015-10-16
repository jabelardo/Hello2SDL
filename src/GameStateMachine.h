//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_GAMESTATEMACHINE_H
#define HELLO2SDL_GAMESTATEMACHINE_H

#include <vector>
#include <memory>

#include "GameState.h"
#include "TextureManager.h"
#include "InputHandler.h"

class GameStateMachine {
public:
  void pushState(std::unique_ptr<GameState> state, TextureManager *textureManager,
                 SDL_Renderer *renderer);

  void changeState(std::unique_ptr<GameState> state, TextureManager *textureManager,
                   SDL_Renderer *renderer);

  void popState(TextureManager *textureManager);

  void update(InputHandler *inputHandler);

  void render(TextureManager *textureManager, SDL_Renderer *renderer);

private:
  std::vector<std::unique_ptr<GameState>> gameStates;

};


#endif //HELLO2SDL_GAMESTATEMACHINE_H