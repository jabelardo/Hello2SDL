#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#endif

#include "TextureManager.cpp"
#include "Game.cpp"
#include "Entity.cpp"

const auto FPS = 60;
const auto DELAY_TIME = 1000.0f / FPS;

int 
main(int argc, char* args[]) {
  auto textureManager = TextureManager{};
  auto game = Game{};
  if (!game.init("Chapter 1", 100, 100, 640, 480, 0, &textureManager)) {
    return 1;
  }

  auto frameStart = Uint32{};
  auto frameTime = Uint32{};

  while (game.isRunning()) {

    frameStart = SDL_GetTicks();

    game.handleEvents();
    game.update();
    game.render();

    frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < DELAY_TIME) {
      SDL_Delay(DELAY_TIME - frameTime);
    }
  }
  game.clean();

  return 0; 
} 