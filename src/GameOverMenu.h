//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_GAMEOVERMENU_H
#define HELLO2SDL_GAMEOVERMENU_H

struct AnimatedGraphic;
struct GameContext;
struct GameMemory;
struct MenuButton;
struct SDL_Renderer;
struct UserInput;

struct GameOverMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *startPlayButton;
    };
    MenuButton *menuButtons[2];
  };
  AnimatedGraphic *gameOverGraphic;
};

bool initGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext, SDL_Renderer *renderer,
                      GameMemory* gameMemory);

void updateGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext, UserInput* userInput);

void renderGameOverMenu(GameOverMenu *gameOverMenu, SDL_Renderer *renderer);

#endif //HELLO2SDL_GAMEOVERMENU_H
