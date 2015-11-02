//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#include "V2D.h"

enum GameState {
  MAIN_MENU_STATE = 0,
  PLAY_STATE,
  PAUSE_MENU_STATE,
  GAME_OVER_STATE,
  BETWEEN_LEVEL_STATE
};

enum GameStateChange {
  NO_CHANGE = 0,
  START_PLAY,
  EXIT_FROM_GAME,
  MAIN_MENU,
  RESUME_PLAY,
  PAUSE_MENU,
  GAME_OVER,
  BETWEEN_LEVEL
};

struct TextureHashNode;
struct MainMenu;
struct PlayState;
struct PauseMenu;
struct GameOverMenu;

struct GameContext {
  GameState currentState;
  GameStateChange stateChange;
  MainMenu *mainMenu;
  PlayState *playState;
  PauseMenu *pauseMenu;
  GameOverMenu *gameOverMenu;

  TextureHashNode *textureHash[4096];
  TextureHashNode *freeTextureHashNodes;
  float gameWidth;
  float gameHeight;
  V2D cameraPosition;
  int currentLevel;
  float scrollSpeed;
  float pixelsPerMt;
  float dtPerFrame;
  bool isLevelCompleted;
};

#endif //HELLO2SDL_GAME_H
