//
// Created by Jose Gutierrez on 10/18/15.
//

#include "Level.h"
#include "tmx.h"
#include "RenderUtils.h"

//void Layer::draw(SDL_Renderer *renderer) {
//
//}
//
//void Layer::update(GameContext *gameContext) {
//
//}
//
//void Level::draw(SDL_Renderer *renderer) {
//
//}
//
//void Level::update(GameContext *gameContext) {
//
//}

bool foo(const char *tmxFile) {
  auto tmxDoc = tmx_load(tmxFile);
  if (!tmxDoc) {
    return false;
  }
  if (tmxDoc->orient != O_ORT) {
    return false;
  }
//  printf("\n\t" "renderorder=%d", tmxDoc->renderorder);
//  printf("\n\t" "height=%d", tmxDoc->height);
//  printf("\n\t" "width=%d", tmxDoc->width);
//  printf("\n\t" "theight=%d", tmxDoc->tile_height);
//  printf("\n\t" "twidth=%d", tmxDoc->tile_width);
//  printf("\n\t" "bgcol=#%.6X", tmxDoc->backgroundcolor);
  return false;
}

void TileLayer::draw(GameContext *gameContext, SDL_Renderer *renderer) {
  auto x = (int) position.x / tileSize;
  auto y = (int) position.y / tileSize;
  auto x2 = (int) position.x % tileSize;
  auto y2 = (int) position.y % tileSize;
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numColumns; j++) {
      int id = tileIds[i][j + x];
      if (id == 0) {
        continue;
      }
      Tileset *tileset = getTilesetById(id);
      if (tileset == 0) {
        continue;
      }
      id--;
      int textureId = 0;
      auto texture = gameContext->functions.getTexture("textureId");
      auto currentFrame = (id - (tileset->firstGridId - 1)) % tileset->numColumns;
      auto currentRow = (id - (tileset->firstGridId - 1)) / tileset->numColumns;
      auto bitmap = Bitmap{texture, tileSize, tileSize, 0, currentFrame, currentRow};

      drawTile(renderer, 2, 2, (j * tileSize) - x2, (i * tileSize) - y2, &bitmap);
    }
  }
}

void TileLayer::update(GameContext *gameContext) {
  position += velocity;
}

bool TileLayer::init(GameContext *gameContext) {
  return false;
}

Tileset *
TileLayer::getTilesetById(int tileId) {
  for (int i = 0; i < tilesetsSize; i++) {
    if (i + 1 <= tilesetsSize - 1) {
      if (tileId >= tilesets[i].firstGridId && tileId < tilesets[i + 1].firstGridId) {
        return &tilesets[i];
      }
    } else {
      return &tilesets[i];
    }
  }
  return 0;
}
