// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ELEMENTS		100000
#define SLOW_ITERATIONS		10000
#define SET_SIZE			1000
#define MAX_THREADS			1

int a[MAX_ELEMENTS], b[MAX_ELEMENTS], c[MAX_ELEMENTS];
const int kIterations = MAX_ELEMENTS / SET_SIZE;

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
	int *threadID, i, j, k, id, iStart, iEnd;
	threadID = (int*) input;
	id = *threadID;

	// Split the size of the set into the number of threads
	iStart = id * (SET_SIZE/MAX_THREADS);
	iEnd = (id+1) * (SET_SIZE/MAX_THREADS);

	// Increase the amount of actual work
	for(j=0; j<SLOW_ITERATIONS; j++){
		// Do my share of the sets
		for( k = 1; k < kIterations; k++){
			for( i = iStart; i < iEnd; i++ ){
				a[i + k*SET_SIZE] = a[i + (k-1)*SET_SIZE] +1;
			}
		}
	}

	return (void*) NULL;
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}
