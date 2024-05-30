#include <ctype.h> // Include ctype.h for tolower() function
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define numThreads 3
#define maxWordLength 100
#define maxTextLength 1000
char text_files[numThreads][20] = {"words1.txt", "words2.txt", "words3.txt"};
char searchWord[maxWordLength];
int wordIndex[numThreads] = {-1};


void *searchWordInFile(void *threadId) {
  long tid = (long)threadId; //Long because threadID can reach to a very beeg number
  FILE *file = fopen(text_files[tid - 1], "r"); //Opening a specific file

  //If a file does not exist, print an error message. "For Debugging"
  if (file == NULL) { 
    printf("Error opening file %s\n", text_files[tid - 1]);
    pthread_exit(NULL);
    return NULL;
  }

  //For Debugging whether which text file is being search is the right file in order.
  char searchWordLower[maxWordLength];
  strcpy(searchWordLower, searchWord);
  // Convert search word to lowercase
  for (int i = 0; searchWordLower[i]; i++) {
    searchWordLower[i] = tolower(searchWordLower[i]);
  }

  //Take note of index, the length of the word, and the number of lines in the file
  int index = 0;
  char word[maxWordLength];
  while (fscanf(file, "%s", word) != EOF) {
    // Convert word to lowercase
    char wordLower[maxWordLength];
    strcpy(wordLower, word);
    for (int i = 0; wordLower[i]; i++) {
      wordLower[i] = tolower(wordLower[i]);
    }

    // Compare lowercase word with lowercase search word
    if (strcmp(wordLower, searchWordLower) == 0) {
      wordIndex[tid - 1] = index;
      printf("Word found in file %s at line %d\n", text_files[tid - 1], index);
      fclose(file);
      pthread_exit(NULL);
    }
    index++; //move to the next word in the text file if they are not the same.
  }

  //prompt if the the loop ends and the word is not found
  printf("Word not found in file %s\n", text_files[tid - 1]);
  fclose(file);
  pthread_exit(NULL);
}

//MAIN
int main(void) {
  pthread_t threads[numThreads];
  int rc;
  long t;

  for (t = 0; t < numThreads; t++) {
    rc = pthread_create(&threads[t], NULL, searchWordInFile, (void *)(t + 1));
    //For debugging, if order or parameters, functions, and arguments when making a thread is correct.
    if (rc) {
      printf("Error: cannot create thread, %d\n", rc);
      exit(-1);
    }

    // Wait for the thread to complete its search before moving to the next one
    pthread_join(threads[t], NULL);
  }

  pthread_exit(NULL);
}
