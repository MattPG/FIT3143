// MATTHEW GREENWOOD 23424303
#include <sys/shm.h> //shm
#include <sys/sem.h> //semaphores
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <errno.h> //perror

// HEADER
// Gets the next two characters to be analysed
void getNextChar(char *prevChar, char *currChar, int pointerid, int *p, char *shm);
// incCounter increments a shared counter by amount
void incCounter(int amount, key_t ckey);
// die prints out a descriptive error message then quits the program
void die(char* s);

// set global int for semaphore
int semid;

// creating the semaphore structs
struct sembuf c_lock = { 0, -1, 0};
struct sembuf c_unlock = { 0, 1, 0};
struct sembuf p_lock = { 1, -1, 0};
struct sembuf p_unlock = { 1, 1, 0};

int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt", *shm, currChar, prevChar;
	int *counter, *pointer, numWords;
	int fileSize, readSize, maxSize, shmid, charSize, counterid, pointerid;
	key_t key = 5343, ckey = 15, pkey = 5, skey = 21;
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

	   // create memory that can hold it all
	   // if successful it returns a non-zero int
	   if((shmid = shmget(key, maxSize, IPC_CREAT | IPC_EXCL | 0666))<0)
		   // Call an error if the id is negative
		   die("shmget_shmid");

	    // attaches the requested memory to the address space of the server
	    if((shm = shmat(shmid, NULL, 0)) == (char*) -1)
	    	// Call an error if the return is -1
			die("shmat_shm");

	   // Place the text into shared memory
	   readSize = fread(shm, charSize, fileSize, inputFile);

	   // check readSize was allocated correctly
	   if (fileSize != readSize)
		   die("Parsed sizes don't match!");

	   // add string termination symbol
	   shm[fileSize+1] = '\0';

	   // create the shared memory counter
	   if((counterid = shmget(ckey, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
		   die("shmget_counterid");

	   // attach counter
	   if((counter = shmat(counterid, NULL, 0)) == (void*)-1)
			   die("shmat_counter");

	   // initialise counter to zero
	   *counter = 0;

	    // request memory for the pointer
	    if((pointerid = shmget(pkey, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
	 	   // Call an error if the id is negative
	 	   die("shmget_pointerid");

	 	// attach pointer to memory
	    if((pointer = shmat(pointerid, NULL, 0)) == (void*) -1)
	 		// Call an error if the return is -1
	 		die("shmat_pointer");

	    // initialise the value of the pointer
	    *pointer = 0;

	    // create the semaphore to lock the counter and pointer
	    if((semid = semget(skey, 2, 0666 | IPC_CREAT))==-1)
	    	die("semget");

	    // initialise the semaphores
	    if((semctl(semid, 0, SETVAL, 1))==-1)
	    	die("semctl_setting0");
	    if((semctl(semid, 1, SETVAL, 1))==-1)
	    	die("semctl_setting1");

	    // FORK HERE 2^3 = 8
	    if(fork() == -1)
	    	die("fork_1");
	    if(fork() == -1)
	    	die("fork_2");
	    if(fork() == -1)
	    	die("fork_3");

		  // Start parsing for words
		   numWords = 0;
		   do {
			   getNextChar(&prevChar, &currChar, pointerid, pointer, shm);
			   // Check if current char is a space
			   if(currChar == ' ' || currChar == '\n' || currChar == '\r'
					   || currChar == '\t'|| currChar == '\0')
				   // Check if previous char was non-space
				   if(prevChar != ' ' && prevChar != '\n' && prevChar != '\r'
						   && prevChar != '\t' && prevChar != '\0')

					   numWords++; //increment word count

		   }while(currChar != '\0');

		   printf("Adding %i words...\n", numWords);

		   // get access to and increment the shared counter
		   incCounter(numWords, ckey);

	    if(prevChar != '\0'){
	    	sleep(1);
		   // print counter value
		   printf("Total number of words: %i\n",*counter);

		   // remove attachment of the shared memory
		   if(shmdt(shm)==-1)
			  die("shmdt_shm");

		   // remove memory allocation
		   if(shmctl(shmid, IPC_RMID, 0)==-1)
			   die("shmctl_shm");

		   // detach counter
		   if(shmdt(counter)==-1)
			   die("shmdt_counter");

		   // mark counter for destruction
		   if(shmctl(counterid, IPC_RMID, 0)==-1)
			   die("shmctl_counter");

		   // detach pointer
		   if(shmdt(pointer)==-1)
			   die("shmdt_counter");

		   // mark pointer for destruction
		   if(shmctl(pointerid, IPC_RMID, 0)==-1)
			   die("shmctl_counter");

		   // mark semaphore for destruction
		   if((semctl(semid, 0, IPC_RMID))==-1)
			   die("semctl_destroy");
	    }

	   exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
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
void getNextChar(char *prevChar, char *currChar, int pointerid, int *p, char *shm) {
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
