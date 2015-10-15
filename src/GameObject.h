//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_GAMEOBJECT_H
#define HELLO2SDL_GAMEOBJECT_H

class GameObject {
  int textureId;
  int currentFrame;
  int currentRow;
  int x;
  int y;
  int width;
  int height;

public:
  void load(int textureId, int x, int y, int width, int height);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);
  
  void update();
  
  void clean();
};


#endif //HELLO2SDL_GAMEOBJECT_H
