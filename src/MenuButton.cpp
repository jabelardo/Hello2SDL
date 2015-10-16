//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"
#include "InputHandler.h"

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
MenuButton::update(InputHandler *inputHandler) {
  auto mousePosition = inputHandler->getMousePosition();
  if (mousePosition.x < (sprite.getPosition().x + sprite.getWidth())
     && mousePosition.x > sprite.getPosition().x
     && mousePosition.y < (sprite.getPosition().y + sprite.getHeight())
     && mousePosition.y > sprite.getPosition().y) {

    if (inputHandler->getMouseButtonState(InputHandler::LEFT_MOUSE_BUTTON) && buttonReleased) {
      sprite.setCurrentFrame(CLICKED);
      callback();
      buttonReleased = false;
    } else if (inputHandler->getMouseButtonState(InputHandler::LEFT_MOUSE_BUTTON)) {
      buttonReleased = true;
      sprite.setCurrentFrame(MOUSE_OVER);
    }
  } else {
    sprite.setCurrentFrame(MOUSE_OUT);
  }
}
