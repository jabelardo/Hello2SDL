//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include <string.h>

enum MemoryPartitionType {
  PERMANENT_MEMORY = 0,
  LONG_TIME_MEMORY,
  SHORT_TIME_MEMORY,
};

struct MemoryPartition {
  MemoryPartitionType type;
  size_t totalSize;
  size_t usedSize;
  void *base;
};

enum GameState {
  NOT_INITIALIZED_STATE = 0,
  PLAY_STATE,
  MAIN_MENU_STATE,
  PAUSE_MENU_STATE,
  GAME_OVER_STATE
};

enum GameStateChange {
  NONE = 0,
  START_PLAY,
  EXIT_FROM_GAME,
  MAIN_MENU,
  RESUME_PLAY,
  PAUSE_MENU,
  GAME_OVER
};

struct ButtonState {
  int halfTransitionCount;
  bool endedDown;
  bool wasDown;
};

struct UserInput {
  bool shouldQuit;

  int mousePositionX;
  int mousePositionY;
  int mouseWheelY;
  ButtonState mouseButtonLeft;
  ButtonState mouseButtonMiddle;
  ButtonState mouseButtonRight;

  ButtonState moveUp;
  ButtonState moveDown;
  ButtonState moveLeft;
  ButtonState moveRight;

  ButtonState actionUp;
  ButtonState actionDown;
  ButtonState actionLeft;
  ButtonState actionRight;

  ButtonState leftShoulder;
  ButtonState rightShoulder;

  ButtonState back;
  ButtonState start;
};

struct PlatformFunctions {
};

struct TextureHashNode {
  char *name;
  SDL_Texture *texture;
  TextureHashNode *next;
};

struct MainMenu;
struct PlayState;
struct PauseMenu;
struct GameOverMenu;

struct GameContext {
  char* resourcePath;
  int screenWidth;
  int screenHeight;
  SDL_Renderer *renderer;
  MemoryPartition permanentMemory;
  MemoryPartition longTimeMemory;
  MemoryPartition shortTimeMemory;
  PlatformFunctions functions;
  GameState currentState;
  GameStateChange stateChange;
  UserInput userInput;
  TextureHashNode *textureHash[4096];
  TextureHashNode *freeTextureHashNodes;
  MainMenu *mainMenu;
  PlayState *playState;
  PauseMenu *pauseMenu;
  GameOverMenu *gameOverMenu;
};

void * reserveMemory(MemoryPartition *partition, size_t reserveSize);

bool freeMemory(MemoryPartition *partition, void* memory);

char * stringConcat(const char * str1, const char * str2, MemoryPartition* memoryPartition);

#ifdef __cplusplus
extern "C" {
#endif

int gameUpdateAndRender(GameContext *gameContext);

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_GAME_H
