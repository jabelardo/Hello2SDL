//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_MAINMENU_H
#define HELLO2SDL_MAINMENU_H

struct GameContext;
struct GameMemory;
struct MenuButton;
struct SDL_Renderer;
struct UserInput;

struct MainMenu {
  union {
    struct {
      MenuButton *startPlayButton;
      MenuButton *exitButton;
    };
    MenuButton *menuButtons[2];
  };
};

bool initMainMenu(MainMenu *mainMenu, GameContext *gameContext, SDL_Renderer *renderer, GameMemory* gameMemory);

void updateMainMenu(MainMenu *mainMenu, GameContext *gameContext, UserInput* userInput);

void renderMainMenu(MainMenu *mainMenu, GameContext *gameContext);

#endif //HELLO2SDL_MAINMENU_H
