// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define NUM_THREADS		8

// HEADER
// method a pthread calls to parse the string
void* startParse(void* input);
// error messenger
void die(char* s);

// Declare global counter and string
char *string;
int fileSize, spacing;


int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt";
	int readSize, charSize, totalCount = 0;
	long threadID;
	pthread_t threads[NUM_THREADS];
	FILE *inputFile;

	charSize = sizeof(char);
	inputFile = fopen(fileName,"r");

	if (inputFile) {

	   // seek the last byte of the file
	   fseek(inputFile,0,SEEK_END);
	   // number of characters in file
	   fileSize = ftell(inputFile);
	   // go back to the start of the file
	   rewind(inputFile);

	   // create memory that can hold it all
	   string = malloc(fileSize);

	   // Place the text into shared memory
	   readSize = fread(string, charSize, fileSize, inputFile);

	   // close the input file
	   fclose(inputFile);

	   // check no errors occurred in reading file
	   if (fileSize != readSize)
		   die("Parsed sizes don't match!");

	   // compute the spacing between each thread
	   spacing = (int) ceil((double)(fileSize) / NUM_THREADS);

	   // create threads
	   printf("Creating threads...\n");
	   for(threadID=0; threadID < NUM_THREADS; threadID++){
	     if (pthread_create(&threads[threadID], NULL, startParse, (void*) threadID)){
	       die("pthread_create " + threadID);
	       }
	     }

	   // wait for all threads to finish
	   int *value = malloc(sizeof(int));
	   for(threadID = 0; threadID<NUM_THREADS; threadID++){
		   pthread_join(threads[threadID], value);
		   totalCount += *value;
	   }

	   // print the total value
	   printf("Total number of words: %i\n", totalCount);

	   // free the allocated memory
	   free(string);

	   exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}


// Start parsing for words
void* startParse(void* input){
	// declare local string pointers
	char currChar, prevChar;
	int currPointer, endPointer, counter;

	// get this thread's ID
	long threadID = (long) input;
	printf("Thread #%ld created...\n", threadID);

	// initialise end pointer
	endPointer = (threadID == (NUM_THREADS -1)) ? (fileSize) : (spacing * (threadID+1) -1);

	// initialise the local word count
	counter = 0;

	// iterate through string
	for(currPointer = spacing * threadID; currPointer <= endPointer; currPointer++){
		currChar = string[currPointer];
		prevChar = (currPointer == 0) ? ' ' : string[currPointer -1];
		// Check if current char is a space
		if(currChar == ' ' || currChar == '\n' || currChar == '\r'
				|| currChar == '\t'|| currChar == '\0')
				// Check if previous char was non-space
			   if(prevChar != ' ' && prevChar != '\n' && prevChar != '\r'
					   && prevChar != '\t' && prevChar != '\0')
				   	   //increment word counter
				   	   counter++;
	}

	   // print personal amount
	   printf("Thread #%ld adding %i words...\n",threadID, counter);
	   pthread_exit(counter);
}
