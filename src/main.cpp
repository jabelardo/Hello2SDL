#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#endif

#include "TextureManager.cpp"
#include "Game.cpp"
#include "GameObject.cpp"

int 
main(int argc, char* args[]) {
  auto textureManager = TextureManager{};
  auto game = Game{};
  if (!game.init("Chapter 1", 100, 100, 640, 480, 0, &textureManager)) {
    return 1;
  }
  
  while (game.isRunning()) {
    game.handleEvents();
    game.update();
    game.render();
    SDL_Delay(10);
  }
  game.clean();

  return 0; 
} 