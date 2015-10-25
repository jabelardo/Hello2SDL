//
// Created by Jose Gutierrez on 10/15/15.
//

#include "MenuButton.h"
#include "RenderUtils.h"

void
drawMenuButton(MenuButton* menuButton, SDL_Renderer *renderer) {
  drawBitmap(renderer, menuButton->x, menuButton->y, &menuButton->bitmap);
}

void
updateMenuButton(MenuButton* menuButton, GameContext *gameContext) {
  int mousePositionX = gameContext->userInput.mousePositionX;
  int mousePositionY = gameContext->userInput.mousePositionY;
  if (mousePositionX < (menuButton->x + menuButton->bitmap.width)
      && mousePositionX > menuButton->x
      && mousePositionY < (menuButton->y + menuButton->bitmap.height)
      && mousePositionY > menuButton->y) {

    if (gameContext->userInput.mouseButtonLeft.endedDown && menuButton->buttonReleased) {
      menuButton->bitmap.currentFrame = MenuButton::CLICKED;
      menuButton->buttonReleased = false;
      gameContext->stateChange = menuButton->stateChange;

    } else if (gameContext->userInput.mouseButtonLeft.endedDown) {
      menuButton->bitmap.currentFrame = MenuButton::MOUSE_OVER;
      menuButton->buttonReleased = true;
    }
  } else {
    menuButton->bitmap.currentFrame = MenuButton::MOUSE_OUT;
  }
}
