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
struct SDL_Renderer;
struct ScrollingBackground;
struct TileMap;
struct UserInput;

struct PlayState {
  int tileMapWidth;
  ScrollingBackground *scrollingBackground;
  EntityNode* entityList;
  EntityNode* freeEntities;
  SDL_Texture* liveTexture;
  SDL_Texture* bullet1Texture;
  SDL_Texture* bullet2Texture;
  SDL_Texture* smallExplosionTexture;
  SDL_Texture* explosionTexture;
  SDL_Texture* largeExplosionTexture;
  SDL_Texture* bossExplosionTexture;
  int currentLives;
  Entity* player;
};

bool initPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer, GameMemory* gameMemory);

bool startGame(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer, GameMemory *gameMemory);

void updatePlayState(PlayState *playState, GameContext *gameContext, UserInput* userInput,
                     GameMemory *gameMemory);

void renderPlayState(PlayState *playState, GameContext* gameContext, SDL_Renderer *renderer);

void addPlayerBullet(PlayState* playState, GameMemory *gameMemory, V2D position, V2D velocity);

void addEnemyBullet(PlayState* playState, GameMemory *gameMemory, V2D position, V2D velocity);

#endif //HELLO2SDL_PLAYSTATE_H
