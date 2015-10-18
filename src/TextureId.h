//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_TEXTUREID_H
#define HELLO2SDL_TEXTUREID_H

enum TextureId {
  NULL_TEXTURE = -1,
  GAME_OVER_TEXT,
  PLAY_BUTTON,
  EXIT_BUTTON,
  HELICOPTER,
  HELICOPTER2,
  MAIN_BUTTON,
  RESUME_BUTTON,
  RESTART_BUTTON,
};

TextureId
    getTextureId(const char *);

#endif //HELLO2SDL_TEXTUREID_H
