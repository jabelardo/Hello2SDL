//
// Created by Jose Gutierrez on 10/24/15.
//
#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include "Entity.h"
#include "RenderUtils.h"
#include "SharedDefinitions.h"

bool
checkEntityCollision(Entity *entity1, Entity *entity2) {
  float leftA = entity1->position.x;
  float rightA = entity1->position.x + entity1->bitmap.width;
  float topA = entity1->position.y;
  float bottomA = entity1->position.y + entity1->bitmap.height;
  //Calculate the sides of rect B
  float leftB = entity2->position.x;
  float rightB = entity2->position.x + entity2->bitmap.width;
  float topB = entity2->position.y;
  float bottomB = entity2->position.y + entity2->bitmap.height;
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}


void
drawEntity(Entity *entity, SDL_Renderer *renderer) {
  switch (entity->type) {
    case ENEMY_TYPE:
    case PLAYER_TYPE: {
      if (entity->isDying) {
        if (entity->velocity.x > 0) {
          drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                     entity->angle, 255, 0, 0, SDL_FLIP_HORIZONTAL);

        } else {
          drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                     entity->angle, 255, 0, 0);

        }
      } else if (entity->velocity.x > 0) {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                   entity->angle, entity->alpha, 255, 255, 255, SDL_FLIP_HORIZONTAL);
      } else {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                   entity->angle, entity->alpha, 255, 255, 255);
      }
    }
  }
}


void scroll(int speed) {

}

void 
doDyingAnimation(Entity *entity) {
  // keep scrolling with the map
  //scroll(gameContext->scrollSpeed);
  entity->bitmap.currentFrame = int(((SDL_GetTicks() / (1000 / 3)) % entity->bitmap.totalFrames));
  if(entity->dyingCounter == entity->dyingTime) {
    entity->isDead = true;
  }
  ++entity->dyingCounter; //simple counter, fine with fixed frame rate
}

void
handleAnimation(Entity *entity) {
  // if the player is invulnerable we can flash its alpha to let people know
  if (entity->invulnerable) {
    // invulnerability is finished, set values back
    if (entity->invulnerableCounter == entity->invulnerableTime) {
      entity->invulnerable = false;
      entity->invulnerableCounter = 0;
      entity->alpha = 255;

    }  else {
      // otherwise, flash the alpha on and off

      if (entity->alpha == 255) {
        entity->alphaDown = true;
      } else if (entity->alpha == 125) {
        entity->alphaDown = false;
      }
      if (entity->alphaDown) {
        entity->alpha -= 10;
      } else {
        entity->alpha += 10;
      }
    }
    if (entity->invulnerableCounter < entity->invulnerableTime) {
      ++entity->invulnerableCounter;
    }
  }
  // if the player is not dead then we can change the angle with the velocity to give the impression
  // of a moving helicopter
  if (!entity->isDead) {
    if (entity->velocity.x < 0) {
      entity->angle = -10.0;
    } else if (entity->velocity.y > 0) {
      entity->angle = 10.0;
    } else {
      entity->angle = 0.0;
    }
  }
//  if (entity->isDying) {
  //  doDyingAnimation(entity);
  //} else {
    // our standard animation code - for helicopter propellors
    entity->bitmap.currentFrame = int(((SDL_GetTicks() / (100)) % entity->bitmap.totalFrames));
  //}
}

void
resurrect(Entity *entity) {
  --entity->currentLives;
  entity->position.x= 10;
  entity->position.y = 200;
  entity->isDying = false;
  entity->isDead = false;
  entity->bitmap.currentFrame = 1;
  entity->alpha = 255;
  entity->dyingCounter = 0;
  entity->invulnerable = true;
}

void
updateEntity(Entity *entity, GameContext *gameContext, UserInput *userInput) {
  switch (entity->type) {
    case PLAYER_TYPE: {
#if 0
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      entity->velocity = V2D{0, 0};

      V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
      entity->velocity = (target - entity->position) / 50;

      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
#else
      // if the level is complete then fly off the screen
      if (gameContext->isLevelCompleted) {
        if (entity->position.x >= gameContext->gameWidth) {
          ++gameContext->currentLevel;
          gameContext->stateChange = BETWEEN_LEVEL;
          gameContext->levelComplete = false;

        } else {
          entity->velocity = V2D{3, 0};
          entity->velocity += entity->acceleration;
          entity->position += entity->velocity;
          handleAnimation(entity);
        }
      } else {
        // if the player is not doing its death animation then update it normally
        if (!entity->isDying) {
          V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
          entity->velocity = (target - entity->position) / 50;
          entity->velocity += entity->acceleration;
          entity->position += entity->velocity;
          handleAnimation(entity);
        } else {
          // if the player is doing the death animation

          entity->velocity += V2D{0, .05};
          entity->position += entity->velocity;
          entity->bitmap.currentFrame = int(((SDL_GetTicks() / (100)) % entity->bitmap.totalFrames));
          // if the death animation has completed
          if (entity->dyingCounter == entity->dyingTime) {
            resurrect(entity);
          } else {
            ++entity->dyingCounter;
          }
        }
      }
#endif
    }
      break;

    case ENEMY_TYPE: {
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      if (entity->position.y < 0) {
        entity->velocity = V2D{2, .33f};
        entity->acceleration = V2D{0, .33f};

      } else if (entity->position.y > 400) {
        entity->velocity = V2D{2, -.33f};
        entity->acceleration = V2D{0, -.33f};
      }
      if (entity->position.x > 640) {
        entity->position.x = 0;
      }
      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
    }
      break;
  }
}