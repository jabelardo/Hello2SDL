//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_GAMESTATE_H
#define HELLO2SDL_GAMESTATE_H


class GameState {
public:
  virtual ~GameState() {};
  virtual void update(InputHandler *inputHandler) = 0;
  virtual void render(TextureManager *textureManager, SDL_Renderer *renderer) = 0;
  virtual bool onEnter(TextureManager *textureManager, SDL_Renderer *renderer) = 0;
  virtual bool onExit(TextureManager *textureManager) = 0;
  virtual int getStateId() const = 0;
};


#endif //HELLO2SDL_GAMESTATE_H
