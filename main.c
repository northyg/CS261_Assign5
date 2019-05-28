#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

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
//FIXED: Do the necessary change to make the implementation //case-insensitive
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
            // convert upper to lower case

          if ((c >= 'A' && c <= 'Z'))
          {
            c = tolower(c);
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
    const char* fileName = "input1.txt";
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);

    clock_t timer = clock();

    HashMap* map = hashMapNew(10);

    // --- Concordance code begins here ---
    // Be sure to free the word after you are done with it here.
    FILE* holdFile = fopen(fileName, "r"); // open the file

    char* currentWord;

    currentWord = nextWord(holdFile);
    int* hashMapGet(HashMap* map, const char* key);
    void hashMapPut(HashMap* map, const char* key, int value);


    while(currentWord != NULL)
    {
      if(hashMapContainsKey(map, currentWord) == 1)
      {
        int *intPtr = hashMapGet(map, currentWord);// returns ptr to current value
        *intPtr = *intPtr + 1; // dereferenced int ptr updated +1
      }

      else
      {
        hashMapPut(map, currentWord, 1);
      }
      free(currentWord);
      currentWord = nextWord(holdFile);
    }

 // close file
    fclose(holdFile);

  /*You are provided with a function nextWord() which takes a FILE*, allocates
    memory for the next word in the file, and returns the word. If the end
    of the file is reached, nextWord() will return NULL. It is your job to open the file using fopen(),
    populate the concordance with the words, and close the file with fclose().
    The file name to open should be given as a command
    line argument when running the program. It will default to input1.txt if no
    file name is provided.

    1. Get the next word with nextWord.
    2. If the word is already in the hash map, then increment its number of occurrences.
    3. Otherwise, put the word in the hash map with a count of 1.
    4. Free the word.

    After processing the text file, print all the words and occurrence counts
    in the hash map.
    Please print them in the format of the following example.
    For the input file of: It was the best of times, it was the worst of times.
    Bucket 0 -> (best, 1) ->
    Bucket 1 -> (the, 2) -> (was, 2) -> (it, 2) ->
    Bucket 3 -> (of, 2) ->
    Bucket 5 -> (worst, 1) ->
    Bucket 6 -> (times, 2) ->
    You can build the program with make prog or make and run it with ./prog <filename>,
    where <filename> is the name of a text file like input1.txt.*/
    hashMapPrint(map); // calls the function to print the stuff above


    // --- Concordance code ends here ---



    timer = clock() - timer;
    printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
    printf("Number of links: %d\n", hashMapSize(map));
    printf("Number of buckets: %d\n", hashMapCapacity(map));
    printf("Table load: %f\n", hashMapTableLoad(map));

    hashMapDelete(map);
    return 0;
}
