//
// Created by Jose Gutierrez on 10/15/15.
//
#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include "MenuButton.h"
#include "RenderUtils.h"
#include "SharedDefinitions.h"

void
drawMenuButton(MenuButton* menuButton, SDL_Renderer *renderer) {
  drawBitmap(renderer, menuButton->x, menuButton->y, &menuButton->bitmap);
}

void
updateMenuButton(MenuButton* menuButton, GameContext *gameContext, UserInput* userInput) {
  int mousePositionX = userInput->mousePositionX;
  int mousePositionY = userInput->mousePositionY;
  if (mousePositionX < (menuButton->x + menuButton->bitmap.width)
      && mousePositionX > menuButton->x
      && mousePositionY < (menuButton->y + menuButton->bitmap.height)
      && mousePositionY > menuButton->y) {

    if (menuButton->clickedTime && menuButton->clickedTime + 125 <= SDL_GetTicks()) {
      menuButton->bitmap.currentFrame = MenuButton::CLICKED;
      menuButton->clickedTime = 0;
      gameContext->stateChange = menuButton->stateChange;

    } else if (userInput->mouseButtonLeft.endedDown) {
      menuButton->bitmap.currentFrame = MenuButton::MOUSE_OUT;
      menuButton->clickedTime = SDL_GetTicks();

    } else {
      menuButton->bitmap.currentFrame = MenuButton::MOUSE_OVER;
    }
  } else {
    menuButton->bitmap.currentFrame = MenuButton::MOUSE_OUT;
  }
}
