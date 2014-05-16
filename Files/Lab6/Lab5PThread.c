// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS		1

// HEADER
void die(char* s);
void* threadStart(void* input);

int main(void) {
   // create threads
   printf("Creating threads...\n");
   int i, threadID[NUM_THREADS];
   pthread_t threads[NUM_THREADS];
   for(i=0; i < NUM_THREADS; i++){
	   threadID[i] = i;
	 if (pthread_create(&(threads[i]), NULL, &threadStart, (void*) &(threadID[i])) != 0)
	   die("pthread_create");
   }
   printf("Threads created.\n");

   // wait for all threads to finish
   printf("Waiting for threads to finish...\n");
   for(i = 0; i<NUM_THREADS; i++)
	   pthread_join(threads[i], NULL);
   printf("Threads closed.\n");

   // Display the matrices
   printMatrices(A,Arows,Acols,B,Brows,Bcols,R);

   // Free the allocated arrays
   free(A);
   free(B);
   free(R);

   exit(EXIT_SUCCESS);
}

// Driver for threads
void* threadStart(void* input){
	int threadID;
	threadID = (int) *input;

	printf("Hello from thread#%i\n", threadID);
	return (void*) NULL;
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}
