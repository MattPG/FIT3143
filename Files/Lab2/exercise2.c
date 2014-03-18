/*
 *
 */
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// die prints out a descriptive error message then quits the program
void die(char* s);
void die(char* s) {
    perror(s);	// error message
    exit(1);	// exit call with unsuccessful completion of program
}

int main(void) {
	// declare required variables and pointers
	char* fileName = "input_text.txt";
	char* shm;
	int fileSize, readSize, maxSize, shmid, charSize;
	key_t key = 65631;
	FILE* inputFile;
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
	   printf("num chars: %i",fileSize);

	   // request memory that can hold it all
	   // if successful it returns a non-zero int
	   shmid = shmget(key, maxSize, IPC_CREAT | 0666);
	   // Call an error if the id is negative
	   if (shmid < 0)
	        die("shmget");

	    // attaches the requested memory to the address space of the server
	    shm = shmat(shmid, NULL, 0);
    	// Call an error if the return is -1
		if (shm == (char*) -1)
	        die("shmat");

	   // Place the text into shared memory
	   readSize = fread(shm, charSize, fileSize, inputFile);

	   // check readSize was allocated correctly
	   if (fileSize != readSize) {
		   // throw away the memory
		   free(shm);
		   die("Parsed sizes don't match!");
		}

	   // add string termination symbol
	   shm[fileSize+1] = '\0';
	   puts("Success!");
	} else	puts("File Failed!");

	return 0;
}
