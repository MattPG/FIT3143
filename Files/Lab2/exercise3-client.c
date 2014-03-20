/*
 * exercise3-client.c
 *
 *  Created on: 20/03/2014
 *      Author: matthew
 */
#include <sys/shm.h> //shm
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <errno.h> //perror

// HEADER
// Gets the next two characters to be analysed
void getNextChar(char *prevChar, char *currChar, int *p, char *shm);
// incCounter increments a shared counter by amount
void incCounter(int amount, key_t ckey);
// getMaxSize retrieves the size of the memory to request
int getMaxSize(key_t mkey);
// die prints out a descriptive error message then quits the program
void die(char* s);

// initial function call
int main() {
	// declare required variables and pointers
    int shmid, maxSize, numWords, pid, *p;
    char *shm, currChar, prevChar;
    key_t key = 5343, ckey = 15, mkey = 10, pkey = 5;

    maxSize = getMaxSize(mkey);

    // request memory of size MAXSIZE with key above and place id in shmid
    if ((shmid = shmget(key, maxSize, SHM_RDONLY)) < 0)
    	// if id is negative then throw error
        die("shmget_shmid");

    // attach memory to the address space of this program
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    	// throw error if shmat returns -1
        die("shmat_shm");

    // request the pointer
    if((pid = shmget(pkey, sizeof(int), IPC_CREAT | 0666))<0)
 	   // Call an error if the id is negative
 	   die("shmget_pid");

 	// attach pointer to memory
 	if((p = shmat(pid, NULL, 0)) == (void*) -1)
 		// Call an error if the return is -1
 		die("shmat_p");

	  // Start parsing for words
	   numWords = 0;
	   //prevChar = '\0';
	   //currChar = '\0';
	   getNextChar(&prevChar, &currChar, p, shm);
	   do {
		   getNextChar(&prevChar, &currChar, p, shm);
		   // Check if current char is a space
		   if(currChar == ' ' || currChar == '\n' || currChar == '\0')
			   // Check if previous char was non-space
			   if(prevChar != ' ' && prevChar != '\n' && prevChar != '\0')
				   numWords++; //increment word count

	   }while(currChar != '\0');

	   // get access to and increment the shared counter
	   incCounter(numWords, ckey);

	/*
     *Change the first character of the
     *segment to '$', indicating we have read
     *the segment.
     */ // make sure only the terminating program can do so
    if(currChar == '\0')
    	*shm = '$';

    // exit program with successful completion
    exit(EXIT_SUCCESS);
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

// gets the size of the file being read to request memory
int getMaxSize(key_t mkey){
	int *comm;
	int commid;

   // request memory that can hold maxSize (int)
   // if successful it returns a non-zero int
   if((commid = shmget(mkey, sizeof(int), SHM_RDONLY))<0)
	   // Call an error if the id is negative
	   die("shmget_placeSize");

	// attaches the requested memory to the address space of the server
	if((comm = shmat(commid, NULL, 0)) == (void*) -1)
		// Call an error if the return is -1
		die("shmat_placeSize");

	return *comm;
}

// Increments the shared memory counter by amount
void incCounter(int amount, key_t ckey){
	int counterid, *counter;

   // request an existing counter
   if((counterid = shmget(ckey, sizeof(int), IPC_CREAT | 0666))<0)
	   // Call an error if the id is negative
	   die("shmget_incCounter");

	// attaches the requested memory to the address space of the server
	if((counter = shmat(counterid, NULL, 0)) == (void*) -1)
		// Call an error if the return is -1
		die("shmat_incCounter");

	// lock counter


	// increment counter
	*counter += amount;

	// unlock counter

}

// Gets the next two characters to be analysed
void getNextChar(char *prevChar, char *currChar, int *p, char *shm) {
	// check if pointer has finished
	if(*p != -1){
		// lock pointer


		// get char and increment shared pointer
		*prevChar = (*p == 0) ? ' ' : shm[*p -1];
		*currChar =	shm[*p];

		// check it's not the end of string
		if(shm[*p] != '\0')
			*p = *p +1;
		else
			*p = -1;

		// unlock pointer

	} else {
		*prevChar = '\0';
		*currChar = '\0';
	}
}
