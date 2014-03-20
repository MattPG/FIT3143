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

// incCounter increments a shared counter by amount
void incCounter(int amount);
// getMaxSize retrieves the size of the memory to request
int getMaxSize();
// die prints out a descriptive error message then quits the program
void die(char* s);
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

// initial function call
int main() {
	// declare required variables and pointers
	int numWords;
    int shmid, maxSize;
    char *shm, *currChar, *prevChar;
    key_t key;

    maxSize = getMaxSize();

    // initialise the value of key (same as server)
    key = 5343;

    // request memory of size MAXSIZE with key above and place id in shmid
    if ((shmid = shmget(key, maxSize, SHM_RDONLY)) < 0)
    	// if id is negative then throw error
        die("shmget_shmid");

    // attach memory to the address space of this program
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    	// throw error if shmat returns -1
        die("shmat_shm");

	  // Start parsing for words
	   numWords = 0;
	   prevChar = malloc(sizeof(char));
	   *prevChar = ' ';
	   currChar = shm;
	   while(*(currChar++)) {
		   // Check if current char is a space
		   if(*currChar == ' ' || *currChar == '\n' || *currChar == '\0')
			   // Check if previous char was non-space
			   if(*prevChar != ' ' && *prevChar != '\n')
				   numWords++; //increment word count

		   prevChar = currChar;
	   };

	   // get access to and increment the shared counter
	   incCounter(numWords);

	/*
     *Change the first character of the
     *segment to '$', indicating we have read
     *the segment.
     */
    *shm = '$';

    // exit program with successful completion
    exit(EXIT_SUCCESS);
}

int getMaxSize(){
	key_t key = 10;
	int *comm;
	int commid;

   // request memory that can hold maxSize (int)
   // if successful it returns a non-zero int
   if((commid = shmget(key, sizeof(int), SHM_RDONLY))<0)
	   // Call an error if the id is negative
	   die("shmget_placeSize");

	// attaches the requested memory to the address space of the server
	if((comm = shmat(commid, NULL, 0)) == (void*) -1)
		// Call an error if the return is -1
		die("shmat_placeSize");

	return *comm;
}

void incCounter(int amount){
	key_t ckey = 15;
	int counterid, *counter;

   // request an existing counter
   if((counterid = shmget(ckey, sizeof(int), SHM_RDONLY))<0)
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
