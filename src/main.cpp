#include "Entity.cpp"
#include "Game.cpp"
#include "TextureManager.cpp"

const auto FPS = 60.f;
const auto DELAY_TIME = (Uint32)(1000.0f / FPS);

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