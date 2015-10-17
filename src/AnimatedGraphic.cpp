//
// Created by Jose Gutierrez on 10/16/15.
//

#include <SDL2/SDL_timer.h>
#include "AnimatedGraphic.h"
#include "UserInput.h"

AnimatedGraphic::AnimatedGraphic(const LoaderParams &params, int animationSpeed)
  : sprite(params), animationSpeed(animationSpeed) {
}

void
AnimatedGraphic::clean() {
}

void
AnimatedGraphic::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  sprite.draw(textureManager, renderer);
}

void
AnimatedGraphic::update(UserInput *inputHandler) {
  if (animationSpeed) {
    sprite.setCurrentFrame(
        (int) ((SDL_GetTicks() / (1000 / animationSpeed)) % sprite.getTotalFrames()));
  }
}
