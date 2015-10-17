//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"
#include "UserInput.h"

MenuButton::MenuButton(const LoaderParams &params, Callback* callback)
: sprite(params), callback(callback), buttonReleased(false) {
  sprite.currentFrame = MOUSE_OUT;
}

void
MenuButton::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  sprite.draw(textureManager, renderer);
}

void
MenuButton::update(UserInput *userInput, SDL_Renderer *renderer) {
  auto mousePositionX = userInput->mousePositionX;
  auto mousePositionY = userInput->mousePositionY;
  if (mousePositionX < (sprite.position.x + sprite.width)
     && mousePositionX > sprite.position.x
     && mousePositionY < (sprite.position.y + sprite.height)
     && mousePositionY > sprite.position.y) {

    if (userInput->mouseButtonLeft.endedDown && buttonReleased) {
      sprite.currentFrame = CLICKED;
      callback(renderer);
      buttonReleased = false;
    } else if (userInput->mouseButtonLeft.endedDown) {
      buttonReleased = true;
      sprite.currentFrame = MOUSE_OVER;
    }
  } else {
    sprite.currentFrame = MOUSE_OUT;
  }
}
