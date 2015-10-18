//
// Created by Jose Gutierrez on 10/18/15.
//

#include <string.h>
#include "TextureId.h"

TextureId
getTextureId(const char *str) {
  if (strcmp(str, "GAME_OVER_TEXT") == 0) return GAME_OVER_TEXT;
  if (strcmp(str, "PLAY_BUTTON") == 0) return PLAY_BUTTON;
  if (strcmp(str, "EXIT_BUTTON") == 0) return EXIT_BUTTON;
  if (strcmp(str, "HELICOPTER") == 0) return HELICOPTER;
  if (strcmp(str, "HELICOPTER2") == 0) return HELICOPTER2;
  if (strcmp(str, "MAIN_BUTTON") == 0) return MAIN_BUTTON;
  if (strcmp(str, "RESUME_BUTTON") == 0) return RESUME_BUTTON;
  if (strcmp(str, "RESTART_BUTTON") == 0) return RESTART_BUTTON;
  return NULL_TEXTURE;
}