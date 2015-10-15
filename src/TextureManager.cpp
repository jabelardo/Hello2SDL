//
// Created by Jose Gutierrez on 10/14/15.
//

#include "TextureManager.h"
#include <cassert>

#ifndef ASSETS_FOLDER
#define ASSETS_FOLDER "assets"
#endif

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

static std::string
sdlGetResourcePath(const std::string &filename) {
  static auto baseRes = std::string{};
  if (baseRes.empty()) {
    auto basePathRaw = SDL_GetBasePath();
    if (!basePathRaw) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
      return "";
    }
    auto basePath = std::string{basePathRaw};
    SDL_free(basePathRaw);
    auto pos = basePath.find("build");
    if (pos == std::string::npos) {
      baseRes = basePath + ASSETS_FOLDER + PATH_SEP;
    } else {
      baseRes = basePath.substr(0, pos) + ASSETS_FOLDER + PATH_SEP;
    }

  }
  auto result = baseRes;
  if (!filename.empty()) {
    result += filename;
  }
  return result;
}

bool
TextureManager::load(int textureId, const std::string &fileName, SDL_Renderer *renderer) {
  auto resource = sdlGetResourcePath(fileName);
  if (resource.empty()) {
    return false;
  }
  auto tempSurface = IMG_Load(resource.c_str());
  if (!tempSurface) {
    return false;
  }
  auto texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
  SDL_FreeSurface(tempSurface);
  if (!texture) {
    return false;
  }
  assert(textureMap.count(textureId) == 0);
  return textureMap.insert({textureId, texture}).second;
}

void
TextureManager::draw(int textureId, int x, int y, int width, int height, SDL_Renderer *renderer,
                     SDL_RendererFlip flip) {
  auto srcRect = SDL_Rect{};
  auto destRect = SDL_Rect{};
  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = destRect.w = width;
  srcRect.h = destRect.h = height;
  destRect.x = x;
  destRect.y = y;
  assert(textureMap.count(textureId) == 1);
  SDL_RenderCopyEx(renderer, textureMap[textureId], &srcRect, &destRect, 0, 0, flip);
}

void
TextureManager::drawFrame(int textureId, int x, int y, int width, int height,
                 int currentRow, int currentFrame, SDL_Renderer *renderer,
                 SDL_RendererFlip flip) {
  auto srcRect = SDL_Rect{};
  auto destRect = SDL_Rect{};
  srcRect.x = width * currentFrame;
  srcRect.y = height * (currentRow - 1);
  srcRect.w = destRect.w = width;
  srcRect.h = destRect.h = height;
  destRect.x = x;
  destRect.y = y;
  assert(textureMap.count(textureId) == 1);
  SDL_RenderCopyEx(renderer, textureMap[textureId], &srcRect, &destRect, 0, 0, flip);
}