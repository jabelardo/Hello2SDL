//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_TEXTUREMANAGER_H
#define HELLO2SDL_TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

class TextureManager {
  std::unordered_map<int, SDL_Texture*> textureMap;

public:
  bool load(int textureId, const std::string &fileName, SDL_Renderer *renderer);

  void draw(int textureId, int x, int y, int width, int height, SDL_Renderer *renderer,
            SDL_RendererFlip flip = SDL_FLIP_NONE);

  void drawFrame(int textureId, int x, int y, int width, int height, int currentRow,
                 int currentFrame, SDL_Renderer *renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
};


#endif //HELLO2SDL_TEXTUREMANAGER_H
