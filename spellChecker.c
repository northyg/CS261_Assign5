#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void updatePossibleWords(HashLink* possibleWords, HashLink* newLink, int i);
int levenshtein(char *s1, char *s2);

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            // make case insensitive
            if (c >= 'A' && c <= 'Z')
            {
              c += 32;
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXED: implement - CHECK IT N STUFF

    char* currentWord = nextWord(file);

    while(currentWord != NULL)
      {
        hashMapPut(map, currentWord, 1);
        free(currentWord);
        currentWord = nextWord(file);
    }
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    HashLink possibleWords[5];
    while (!quit)
    {
        // re-initialize values to large numbers
        possibleWords[0].value = 300;
        possibleWords[1].value = 300;
        possibleWords[2].value = 300;
        possibleWords[3].value = 300;
        possibleWords[4].value = 300;
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..

        // Make sure inputBuffer is lower case
        for (int i = 0; inputBuffer[i] != '\0'; ++i)
        {
          if (inputBuffer[i] >= 'A' && inputBuffer[i] <= 'Z')
          {
            inputBuffer[i] += 32;
          }
        }

        printf("The inputted word \"%s\" is spelled ", inputBuffer);
        // see if word is in Dictionary
        if (hashMapContainsKey(map, inputBuffer))
        {
          printf("correctly\n");
        }
        else  // word is NOT in the dictionary
        {
          printf("incorrectly\nDid you mean...?\n");

          // check every bucket
          for(int i = 0; i < map->capacity; i++)
          {
              HashLink* currentLink = map->table[i];
              // check every link in each bucket
              while (currentLink != NULL)
              {
                  // calculate levenshtein value
                  int newValue = levenshtein(inputBuffer, currentLink->key);
                  currentLink->value = newValue;
                  hashMapPut(map, currentLink->key, newValue);

                  // update array of possible answers by comparing
                  // newValue with possiblewords[x].value
                  updatePossibleWords(possibleWords, currentLink, 4);

                  currentLink = currentLink->next;
              }
          }

          // print suggested possibleWords
          for (int i = 0; i < 5; ++i)
          {
            printf("%s\n", possibleWords[i].key);
          }
        }

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}

void updatePossibleWords(HashLink* possibleWords, HashLink* newLink, int i)
{
    if (newLink->value < possibleWords[i].value)
    {
        // If this is not the last item in the array, we need to move it
        // down an index in the pecking order
        if (i < 4)
        {
          possibleWords[i+1].key = possibleWords[i].key;
          possibleWords[i+1].value = possibleWords[i].value;
        }

        // If we are at the last index, we just update it and return
        if (i == 0)
        {
          possibleWords[i].key = newLink->key;
          possibleWords[i].value = newLink->value;
          return;
        }

        // recursively call the function and compare the new value to
        // the next index in the array
        updatePossibleWords(possibleWords, newLink, i-1);
    }
    else
    {
        // this value will not be added to the array if its not smaller
        // than the largest saved value
        if (i < 4)
        {
            // value is not smaller than one at [i], save it into [i+1]
            possibleWords[i+1].key = newLink->key;
            possibleWords[i+1].value = newLink->value;
        }
    }
}

// Levenshtein Distance code Source:
// Algorithm Implementation/Strings/Levenshtein distance
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2) {
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[s2len+1][s1len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

    return(matrix[s2len][s1len]);
}
