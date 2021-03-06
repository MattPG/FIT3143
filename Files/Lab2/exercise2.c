// MATTHEW GREENWOOD 23424303
/*
 * This program reads in a long string of text into shared memory
 * and prints out how many words are in the string.
 */
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// die prints out a descriptive error message then quits the program
void die(char* s);
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt";
	char *shm, *currChar, *prevChar;
	int fileSize, readSize, maxSize, shmid, charSize, numWords;
	key_t key;
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
	   // define the required size to store it
	   maxSize = charSize * (fileSize + 1);

	   // generate ID for key
	   if((key = ftok("input_text.txt",0)) == -1)
		   die("ftok");

	   // request memory that can hold it all
	   // if successful it returns a non-zero int
	   if((shmid = shmget(key, maxSize, IPC_CREAT | 0666))<0)
		   // Call an error if the id is negative
		   die("shmget");

	    // attaches the requested memory to the address space of the server
	    if((shm = shmat(shmid, NULL, 0)) == (char*) -1)
	    	// Call an error if the return is -1
			die("shmat");

	   // Place the text into shared memory
	   readSize = fread(shm, charSize, fileSize, inputFile);

	   // check readSize was allocated correctly
	   if (fileSize != readSize)
		   die("Parsed sizes don't match!");

	   // add string termination symbol
	   shm[fileSize+1] = '\0';

	   // Start parsing for words
	   numWords = 0;
	   prevChar = (char*) malloc(sizeof(char));
	   *prevChar = ' ';
	   currChar = shm;
	   while(*(currChar++)) {
		   // Check if current char is a space
		   if(*currChar == ' ' || *currChar == '\n' || *currChar == '\r'
				 ||  *currChar == '\t' || *currChar == '\0')
			   // Check if previous char was non-space
			   if(*prevChar != ' ' && *prevChar != '\n' &&
					   *prevChar != '\r' && *prevChar != '\r')

				   numWords++; //increment word count

		   prevChar = currChar;
	   };

	   printf("Number of words: %i", numWords);

	   // remove attachment of the shared memory
	   if(shmdt(shm)==-1)
		  die("shmdt");

	   // remove memory allocation
	   if(shmctl(shmid, IPC_RMID, 0)==-1)
		   die("shmctl");


	   exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}
