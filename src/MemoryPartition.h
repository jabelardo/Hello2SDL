//
// Created by Jose Gutierrez on 10/25/15.
//

#ifndef HELLO2SDL_MEMORYPARTITION_H
#define HELLO2SDL_MEMORYPARTITION_H

#include <stddef.h>

enum MemoryPartitionType {
  PERMANENT_MEMORY = 0,
  LONG_TIME_MEMORY,
  SHORT_TIME_MEMORY,
};

struct MemoryPartition {
  MemoryPartitionType type;
  size_t totalSize;
  size_t usedSize;
  void *base;
};

void * reserveMemory(MemoryPartition *partition, size_t reserveSize);

#define RESERVE_MEMORY(PARTITION, TYPE) (TYPE *) reserveMemory(PARTITION, sizeof(TYPE))

bool freeMemory(MemoryPartition *partition, void* memory);

char * stringConcat(const char * str1, const char * str2, MemoryPartition* memoryPartition);

#endif //HELLO2SDL_MEMORYPARTITION_H
