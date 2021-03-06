// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ELEMENTS	100000
#define ITERATIONS		10000
#define MAX_THREADS		4

int a[MAX_ELEMENTS], b[MAX_ELEMENTS], c[MAX_ELEMENTS];

// HEADER
void die(char* s);
void* threadStart(void* input);

int main(void) {

	printf("Starting with %i thread(s)...\n", MAX_THREADS);

   // create arrays for thread info
   int i, threadID[MAX_THREADS];
   pthread_t threads[MAX_THREADS];

   // create threads
   for(i=0; i < MAX_THREADS; i++){
	   threadID[i] = i;
	 if (pthread_create(&(threads[i]), NULL, &threadStart, (void*) &(threadID[i])) != 0)
	   die("pthread_create");
   }

   // wait for all threads to finish
   for(i = 0; i<MAX_THREADS; i++)
	   pthread_join(threads[i], NULL);

	printf("...Finished with %i thread(s)\n", MAX_THREADS);

   exit(EXIT_SUCCESS);
}

// Driver for threads
void* threadStart(void* input){
	int *threadID, i, j, id;
	threadID = (int*) input;
	id = *threadID;

	for(j=0; j<ITERATIONS; j++){
		for(i=id * (MAX_ELEMENTS/MAX_THREADS); i < (id+1) * (MAX_ELEMENTS/MAX_THREADS) ; i++){
			a[i] = b[i] + c[i];
		}
	}

	return (void*) NULL;
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}
