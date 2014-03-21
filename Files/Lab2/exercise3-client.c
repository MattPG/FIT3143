// MATTHEW GREENWOOD 23424303
/*
 * exercise3-client.c
 *
 *  Created on: 20/03/2014
 *      Author: matthew
 */
#include <sys/shm.h> //shm
#include <sys/sem.h> //semaphores
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <errno.h> //perror

// set global int for semaphore
int semid;

// creating the semaphore structs
struct sembuf c_lock = { 0, -1, 0};
struct sembuf c_unlock = { 0, 1, 0};
struct sembuf p_lock = { 1, -1, 0};
struct sembuf p_unlock = { 1, 1, 0};

// HEADER
// Gets the next two characters to be analysed
void getNextChar(char *prevChar, char *currChar, int pid, int *p, char *shm);
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
    key_t key = 5343, ckey = 15, mkey = 10, pkey = 5, skey = 21;

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

    // get the semaphore to lock the counter and pointer
    semid = semget(skey, 2, 0666 | IPC_CREAT);

	  // Start parsing for words
	   numWords = 0;
	   do {
		   getNextChar(&prevChar, &currChar, pid, p, shm);
		   // Check if current char is a space
		   if(currChar == ' ' || currChar == '\n' || currChar == '\r'
				   || currChar == '\t'|| currChar == '\0')
			   // Check if previous char was non-space
			   if(prevChar != ' ' && prevChar != '\n' && prevChar != '\r'
					   && prevChar != '\t' && prevChar != '\0')

				   numWords++; //increment word count

	   }while(prevChar != '\0');

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
	if(semop(semid, &c_lock, 1)==-1)
		die("semctl_lock_incCounter");
	// increment counter
	*counter += amount;

	// unlock counter
	if(semop(semid, &c_unlock, 1)==-1)
		die("semctl_lock_incCounter");
}

// Gets the next two characters to be analysed
void getNextChar(char *prevChar, char *currChar, int pid, int *p, char *shm) {
	// lock pointer
	if(semop(semid, &p_lock, 1)==-1)
		die("semctl_lock_pointer");

	// check if pointer has finished
	if(*p != -1){

		// get char and increment shared pointer
		*prevChar = (*p == 0) ? ' ' : shm[*p -1];
		*currChar =	shm[*p];

		// check it's not the end of string
		if(shm[*p] != '\0')
			*p = *p +1;
		else
			*p = -1;

	} else {
		*prevChar = '\0';
		*currChar = '\0';
	}

	// unlock pointer
	if(semop(semid, &p_unlock, 1)==-1)
		die("semctl_unlock_pointer");
}
