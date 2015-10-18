//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"
#include "RenderUtils.h"

void
MenuButton::draw(SDL_Renderer *renderer) {
  drawBitmap(renderer, x, y, &bitmap);
}

void
MenuButton::update(GameContext *gameContext) {
  auto mousePositionX = gameContext->userInput->mousePositionX;
  auto mousePositionY = gameContext->userInput->mousePositionY;
  if (mousePositionX < (x + bitmap.width)
      && mousePositionX > x
      && mousePositionY < (y + bitmap.height)
      && mousePositionY > y) {

    if (gameContext->userInput->mouseButtonLeft.endedDown && buttonReleased) {
      bitmap.currentFrame = CLICKED;
      callback(gameContext);
      buttonReleased = false;
    } else if (gameContext->userInput->mouseButtonLeft.endedDown) {
      buttonReleased = true;
      bitmap.currentFrame = MOUSE_OVER;
    }
  } else {
    bitmap.currentFrame = MOUSE_OUT;
  }
}
