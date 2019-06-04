/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Giselle Northy
 * Date: 5/25/2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* HashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with HashLinkNew.
 * @param link
 */
static void HashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * HashLinkDelete to free the links.
 * @param map
 */

 // FIXED: implement

void hashMapCleanUp(HashMap* map)
{
  for (int i = 0; i < map->capacity; i++)
  {
      if(map->table[i] != NULL) // if it not NULL, then there is a linked list here
      {
        // delete stuff in buckets
        struct HashLink* tempLink = map->table[i]; // point 1st link
        struct HashLink* deleteLink = NULL; // temporary pointer
        while(tempLink != NULL)
        {
          deleteLink = tempLink;
          tempLink = tempLink->next; // look at next one
          HashLinkDelete(deleteLink); // call delete
        }
      }
  }
}


/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXED: implement
    int index = HASH_FUNCTION(key) % map->capacity;
    if(index < 0)
    {
      index += map->capacity; // index to where the list will be
    }

    struct HashLink* tempLink = map->table[index];


    while(tempLink != NULL)
    {
      if(strcmp(tempLink->key, key) == 0) // if tempLink matches then returns 0
      {
        return &tempLink->value;
      }
      // if not found, it keeps looking and assigns to next...
      tempLink = tempLink->next;
    }
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity (double of the old capacity). After allocating the new table,
 * all of the links need to rehashed into it because the capacity has changed.
 *
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 *
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXED: implement
    struct HashMap* newMap = hashMapNew(capacity); // create new table

    // copy values from old to new table
    for (int i = 0; i < map->capacity; i++)
    {
        if(map->table[i] != NULL) // if it not NULL, then there is a linked list here
        {
          //
          struct HashLink* tempLink = map->table[i]; // point 1st link
          while(tempLink != NULL)
          {
            hashMapPut(newMap, tempLink->key, tempLink->value);
            tempLink = tempLink->next; // look at next one
          }
        }
    }
    hashMapCleanUp(map);
    map->table = newMap->table;
    map->size = newMap->size;
    map->capacity = newMap->capacity;
    free(newMap);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use HashLinkNew to create the link.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 *
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXED: implement
    int* address = hashMapGet(map, key);
    if(address == NULL) // if null, key does not exist yet
    {
      // thus create new link key to value

      // first see if we need to resize the table
      if (hashMapTableLoad(map) >= (float)MAX_TABLE_LOAD)
      {
        resizeTable(map, (map->capacity * 2));
      }

      // find where to put it
      int index = HASH_FUNCTION(key) % map->capacity;
      if(index < 0)
      {
        index += map->capacity; // index to where the list will be
      }
      // creates a new link and puts it in the table "bucket"
      map->table[index] = HashLinkNew(key, value, map->table[index]);
      ++map->size;
    }
    else
    {
      // if found, update the key's value
      *address = value;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use HashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXED: implement

    int index = HASH_FUNCTION(key) % map->capacity;
    if(index < 0)
    {
      index += map->capacity; // index to where the list will be
    }
    struct HashLink* tempLink = map->table[index];
    struct HashLink* prevLink = NULL;
    while(tempLink != NULL)
    {
      if(strcmp(tempLink->key, key) == 0) // if tempLink matches then returns 0
      {
        if(prevLink != NULL)
        {
          // this link won't be the 1st
          prevLink->next = tempLink->next;
        }
        else
        {
          // this link it is the 1st, deleting link 1
          map->table[index] = tempLink->next;
        }
        HashLinkDelete(tempLink);
        --map->size;
        break;
      }
      // if not found, it keeps looking and assigns to next...
      prevLink = tempLink;
      tempLink = tempLink->next;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXED: implement
    int index = HASH_FUNCTION(key) % map->capacity;
    if(index < 0)
    {
      index += map->capacity; // index to where the list will be
    }
    struct HashLink* tempLink = map->table[index];
    while(tempLink != NULL)
    {
      if(strcmp(tempLink->key, key) == 0) // if tempLink matches then returns 0
      {
        return 1;
      }
      // if not found, it keeps looking and assigns to next...
      tempLink = tempLink->next;
    }

    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXED: implement
    return map->size;

}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
  // FIXED
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXED: implement
    int nullBuckets = 0;

    for(int i = 0; i < map->capacity; i++)
    {
      if(map->table[i] == NULL)
      {
        ++nullBuckets;
      }
    }
    return nullBuckets;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXED: implement
    float ratio;
    // doing (float) in front is casting them to a float
    ratio = (float)map->size / (float)map->capacity;

    return ratio;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXED: implement
  // links is "size"
  for(int i = 0; i < map->capacity; i++) // capacity goes thru # of buckets
  {
    if (map->table[i] != NULL)
    {
      printf("\nBucket %d -> ", i);
      struct HashLink* tempLink = map->table[i];

      while(tempLink != NULL)
      {
        printf("(%s, %d) -> ", tempLink->key, tempLink->value);
        tempLink = tempLink->next;
      }
    }
  }
  printf("\n");
}
