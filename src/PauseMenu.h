//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_PAUSEMENU_H
#define HELLO2SDL_PAUSEMENU_H

struct GameContext;
struct GameMemory;
struct MenuButton;
struct PlatformConfig;
struct SDL_Renderer;
struct UserInput;

struct PauseMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *resumePlayButton;
    };
    MenuButton *menuButtons[2];
  };
};

bool initPauseMenu(PauseMenu *pauseMenu, GameContext *gameContext, SDL_Renderer *renderer,
                   GameMemory* gameMemory, PlatformConfig *platformConfign);

void updatePauseMenu(PauseMenu *pauseMenu, GameContext *gameContext, UserInput* userInput);

void renderPauseMenu(PauseMenu *pauseMenu, SDL_Renderer *renderer);

#endif //HELLO2SDL_PAUSEMENU_H
