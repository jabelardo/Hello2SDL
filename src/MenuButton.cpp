//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"

enum MouseButtonState {
  MOUSE_OUT = 0,
  MOUSE_OVER,
  CLICKED
};

MenuButton::MenuButton(const Sprite &params, CallbackFunc* callback)
: sprite(params), callback(callback), buttonReleased(false) {
  sprite.currentFrame = MOUSE_OUT;
}

void
MenuButton::draw(SDL_Renderer* renderer) {
  sprite.draw(renderer);
}

void
MenuButton::update(GameContext* gameContext) {
  auto mousePositionX = gameContext->userInput->mousePositionX;
  auto mousePositionY = gameContext->userInput->mousePositionY;
  if (mousePositionX < (sprite.position.x + sprite.width)
     && mousePositionX > sprite.position.x
     && mousePositionY < (sprite.position.y + sprite.height)
     && mousePositionY > sprite.position.y) {

    if (gameContext->userInput->mouseButtonLeft.endedDown && buttonReleased) {
      sprite.currentFrame = CLICKED;
      callback(gameContext);
      buttonReleased = false;
    } else if (gameContext->userInput->mouseButtonLeft.endedDown) {
      buttonReleased = true;
      sprite.currentFrame = MOUSE_OVER;
    }
  } else {
    sprite.currentFrame = MOUSE_OUT;
  }
}
