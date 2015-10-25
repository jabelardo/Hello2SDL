//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

struct Entity;
struct GameContext;
struct GameMemory;
struct MenuButton;
struct PlatformConfig;
struct SDL_Renderer;
struct TileMap;
struct UserInput;

struct PlayState {
  TileMap *tileMap;
  Entity *enemy;
};

bool initPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer,
                   GameMemory* gameMemory, PlatformConfig *platformConfig);

void updatePlayState(PlayState *playState, GameContext *gameContext, UserInput* userInput);

void renderPlayState(PlayState *playState, SDL_Renderer *renderer);

bool startGame(PlayState *playState, GameContext *gameContext);

#endif //HELLO2SDL_PLAYSTATE_H
