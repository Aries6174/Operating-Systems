#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define numThreads 3
#define maxWordLength 100
#define maxTextLength 1000

// Hashmap node structure
typedef struct Node {
  char word[maxWordLength];
  struct Node *next;
} Node;

// Global variables
char text_files[numThreads][20] = {"words1.txt", "words2.txt", "words3.txt"};
Node *hashmap[10000];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int totalWords = 0;

// Function to calculate the hash value
unsigned int hash(char *word) {
  unsigned int hash = 0;
  while (*word) {
    hash = (hash << 5) + *word++;
  }
  return hash % 10000; // Modulo to fit into the hashmap array
}

// Function to insert a word into the hashmap
void insertWord(char *word) {
  unsigned int index = hash(word);
  pthread_mutex_lock(&mutex);
  Node *curr = hashmap[index];
  while (curr != NULL) {
    if (strcmp(curr->word, word) == 0) {
      pthread_mutex_unlock(&mutex);
      return;
    }
    curr = curr->next;
  }
  Node *newNode = (Node *)malloc(sizeof(Node));
  strcpy(newNode->word, word);
  newNode->next = hashmap[index];
  hashmap[index] = newNode;
  totalWords++; // Increment total words count
  pthread_mutex_unlock(&mutex);
}

// Function to search for a word in a file
void *searchWordInFile(void *threadId) {
  long tid = (long)threadId;
  FILE *file = fopen(text_files[tid - 1], "r");
  if (file == NULL) {
    printf("Error opening file %s\n", text_files[tid - 1]);
    pthread_exit(NULL);
    return NULL;
  }

  char word[maxWordLength];
  while (fscanf(file, "%s", word) != EOF) {
    // Convert word to lowercase
    for (int i = 0; word[i]; i++) {
      word[i] = tolower(word[i]);
    }
    insertWord(word);
  }

  fclose(file);
  pthread_exit(NULL);
}

// Main function
int main(void) {
  pthread_t threads[numThreads];
  int rc;
  long t;

  // Create threads for searching words in files
  for (t = 0; t < numThreads; t++) {
    rc = pthread_create(&threads[t], NULL, searchWordInFile, (void *)(t + 1));
    if (rc) {
      printf("Error: cannot create thread, %d\n", rc);
      exit(-1);
    }
  }

  // Wait for all threads to complete
  for (t = 0; t < numThreads; t++) {
    pthread_join(threads[t], NULL);
  }

  // Print total number of unique words
  printf("The length of the Hashmap is: %d\n", totalWords);

  pthread_exit(NULL);
}
