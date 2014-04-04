// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS		8

// Declare global counter and string
char *string;
int *counter[NUM_THREADS],fileSize;

// HEADER
// method a pthread calls to parse the string
void *startParse(void* input);
// error messenger
void die(char* s);

int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt";
	int fileSize, readSize, charSize;
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
	   string = malloc(fileSize*charSize);

	   // Place the text into shared memory
	   readSize = fread(string, charSize, fileSize, inputFile);

	   // check no errors occurred in reading file
	   if (fileSize != readSize)
		   die("Parsed sizes don't match!");

	   // add string termination symbol
	   string[fileSize+1] = '\0';

	   // create pthreads here
	   printf("Creating threads...\n");
	   for(threadID=0; threadID < NUM_THREADS; threadID++){
	     if (pthread_create(&threads[threadID], NULL, startParse, (void*) threadID)){
	       die("pthread_create " + threadID);
	       }
	     }

	   // wait for all threads to finish

	   // print the total value

	   pthread_exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}


// Start parsing for words
void *startParse(void* input){
	// Local word counter
	long threadID = (long) input;
	printf("Thread #%ld created.\n", threadID);
	counter[threadID] = 0;

//	   do {
//		   getNextChar(&prevChar, &currChar, pointerid, pointer, shm);
//		   // Check if current char is a space
//		   if(currChar == ' ' || currChar == '\n' || currChar == '\r'
//				   || currChar == '\t'|| currChar == '\0')
//			   // Check if previous char was non-space
//			   if(prevChar != ' ' && prevChar != '\n' && prevChar != '\r'
//					   && prevChar != '\t' && prevChar != '\0')
//
//				   numWords++; //increment word count
//
//	   }while(currChar != '\0');

	   // print personal amount
	   printf("Adding %ld words...\n", counter[threadID]);

	   // increment total amount
}
