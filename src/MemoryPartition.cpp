//
// Created by Jose Gutierrez on 10/25/15.
//

#include <assert.h>
#include <sys/types.h>
#include "Game.h"
#include "MemoryPartition.h"

template<typename T>
bool
isNegative(T val) {
  int result = (val > T(0)) - (val < T(0));
  return result == -1;
}

void *
reserveMemory(MemoryPartition *partition, size_t reserveSize) {
  if (partition->type == PERMANENT_MEMORY || partition->type == SHORT_TIME_MEMORY) {
    assert(reserveSize <= partition->totalSize - partition->usedSize);
    int8_t *result = (int8_t *) partition->base + partition->usedSize;
    partition->usedSize += reserveSize;
    return result;

  } else if (partition->type == LONG_TIME_MEMORY) {
    void *block = partition->base;
    while (block < (int8_t *) partition->base + partition->totalSize) {
      // NOTE:
      // each memory block is prefixed with a ssize_t value indicating the block size, the block is
      // free to use if the sign of the size header is negative
      ssize_t blockSize = *(ssize_t *) block;
      if (isNegative(blockSize)) {
        if (-1 * blockSize >= reserveSize + sizeof(ssize_t)) {
          assert((int8_t *) block + sizeof(ssize_t) + reserveSize + sizeof(ssize_t)
                 <= (int8_t *) partition->base + partition->totalSize);
          ssize_t *resultAddress = (ssize_t *) block + 1;
          partition->usedSize += reserveSize + sizeof(ssize_t);
          *(ssize_t *) ((int8_t *) resultAddress + reserveSize) =
              blockSize + reserveSize + sizeof(ssize_t);
          *(ssize_t *) block = reserveSize;
          return resultAddress;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    assert(false);
    return 0;
  }
  assert(false);
  return 0;
}

bool
freeMemory(MemoryPartition *partition, void *memory) {
  if (partition->type == PERMANENT_MEMORY) {
    return false;

  } else if (partition->type == SHORT_TIME_MEMORY) {
    return true;

  } else if (partition->type == LONG_TIME_MEMORY) {
    assert(memory >= partition->base);
    assert(memory <= (int8_t *) partition->base + partition->totalSize);
    ssize_t *memorySize = (ssize_t *) memory - 1;

    if (isNegative(*memorySize)) {
      return false;
    }
    partition->usedSize -= *memorySize;
    *memorySize *= -1;

    // join contiguous free blocks
    void *block = partition->base;
    ssize_t *freeBlock = (ssize_t *) 0;
    while (block < (int8_t *) partition->base + partition->totalSize) {
      ssize_t blockSize = *(ssize_t *) block;
      if (isNegative(blockSize)) {
        if (freeBlock) {
          *freeBlock += blockSize - sizeof(ssize_t);
          partition->usedSize += blockSize - sizeof(ssize_t);
        } else {
          freeBlock = (ssize_t *) block;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        freeBlock = 0;
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    return true;
  }
  assert(false);
  return false;
}
