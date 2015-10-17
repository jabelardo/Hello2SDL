//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"
#include "UserInput.h"

MenuButton::MenuButton(const LoaderParams &params, Callback* callback)
: sprite(params), callback(callback), buttonReleased(false) {
  sprite.setCurrentFrame(MOUSE_OUT);
}

void
MenuButton::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  sprite.draw(textureManager, renderer);
}

void
MenuButton::clean() {

}

void
MenuButton::update(UserInput *userInput, SDL_Renderer *renderer) {
  auto mousePositionX = userInput->mousePositionX;
  auto mousePositionY = userInput->mousePositionY;
  if (mousePositionX < (sprite.getPosition().x + sprite.getWidth())
     && mousePositionX > sprite.getPosition().x
     && mousePositionY < (sprite.getPosition().y + sprite.getHeight())
     && mousePositionY > sprite.getPosition().y) {

    if (userInput->mouseButtonLeft.endedDown && buttonReleased) {
      sprite.setCurrentFrame(CLICKED);
      callback(renderer);
      buttonReleased = false;
    } else if (userInput->mouseButtonLeft.endedDown) {
      buttonReleased = true;
      sprite.setCurrentFrame(MOUSE_OVER);
    }
  } else {
    sprite.setCurrentFrame(MOUSE_OUT);
  }
}
