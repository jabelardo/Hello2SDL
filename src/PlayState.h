//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_PLAYSTATE_H
#define HELLO2SDL_PLAYSTATE_H

#include <SDL2/SDL_render.h>
#include "Entity.h"

struct GameContext;
struct GameMemory;
struct MenuButton;
struct PlatformConfig;
struct SDL_Renderer;
struct TileMap;
struct UserInput;

struct EntityNode {
  Entity entity;
  EntityNode *next;
};

struct PlayState {
  TileMap *tileMap;
  EntityNode* enemies;
  SDL_Texture* liveTexture;
  EntityNode* playerBullets;
  EntityNode* enemyBullets;
  EntityNode* freeEntities;
  SDL_Texture* bullet1Texture;
  SDL_Texture *glider;
};

bool initPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer,
                   GameMemory* gameMemory, PlatformConfig *platformConfig);

void updatePlayState(PlayState *playState, GameContext *gameContext, UserInput* userInput,
                     GameMemory *gameMemory);

void renderPlayState(PlayState *playState, SDL_Renderer *renderer);

bool startGame(PlayState *playState, GameContext *gameContext);

void addPlayerBullet(PlayState* playState, GameMemory *gameMemory, V2D position, V2D velocity);

#endif //HELLO2SDL_PLAYSTATE_H
