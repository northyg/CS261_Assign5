#include <stdio.h>
#include "hashMap.h"

int main(int argc, char** argv)
{
  HashMap* map = hashMapNew(10);

  hashMapPut(map, "dog", 234);
  // size should be 1, capacity should be 10
  printf("Size = %d\nCapacity= %d\n", map->size, map->capacity);
  return 0;
}
