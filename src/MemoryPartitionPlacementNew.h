//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_MEMORYPARTITIONPLACEMENTNEW_H
#define HELLO2SDL_MEMORYPARTITIONPLACEMENTNEW_H

int8_t *
reserveMemory(MemoryPartition *permanentMemory, size_t memorySize) {
  assert(memorySize < permanentMemory->totalSize - permanentMemory->usedSize);
  auto result = (int8_t *) permanentMemory->base + permanentMemory->usedSize;
  permanentMemory->usedSize += memorySize;
  return result;
}

#define PLACEMENT_NEW(MEMORY, TYPE) new(reserveMemory(MEMORY, sizeof(TYPE)))

#endif //HELLO2SDL_MEMORYPARTITIONPLACEMENTNEW_H
