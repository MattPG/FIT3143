// MATTHEW GREENWOOD 23424303
#include <sys/shm.h> //shm
#include <sys/sem.h> //semaphores
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <errno.h> //perror

// HEADER
// placeSize puts an int into common shared memory
int* placeMaxSize(int maxSize, int *shmid, key_t mkey);
// die prints out a descriptive error message then quits the program
void die(char* s);

int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt", *shm;
	int *comm, *counter, *pointer;
	int fileSize, readSize, maxSize, shmid, charSize, commid, counterid, pid, semid;
	key_t key = 5343, ckey = 15, mkey = 10, pkey = 5, skey = 21;
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

	   // place the maximum size in common memory to be accessed by clients
	   comm = placeMaxSize(maxSize, &commid, mkey);

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
	    if((pid = shmget(pkey, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
	 	   // Call an error if the id is negative
	 	   die("shmget_pid");

	 	// attach pointer to memory
	    if((pointer = shmat(pid, NULL, 0)) == (void*) -1)
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

	   // wait until other programs finish
	   while(*shm != '$'){
		   sleep(1);
		   semid = semget(key, 10, 0666 | IPC_CREAT);
		   printf("Waiting for other programs!\n");
	   }

	   sleep(3);
	   // print counter value
	   printf("Total number of words: %i\n",*counter);

	   // remove attachment of the shared memory
	   if(shmdt(shm)==-1)
		  die("shmdt_shm");

	   // remove memory allocation
	   if(shmctl(shmid, IPC_RMID, 0)==-1)
		   die("shmctl_shm");

	   // remove attachment of the common memory (maxSize)
	   if(shmdt(comm)==-1)
		  die("shmdt_comm");

	   // mark memory for dest (maxSize)
	   if(shmctl(commid, IPC_RMID, 0)==-1)
		   die("shmctl_commid");

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
	   if(shmctl(pid, IPC_RMID, 0)==-1)
		   die("shmctl_counter");

	   // mark semaphore for destruction
	   if((semctl(semid, 0, IPC_RMID))==-1)
		   die("semctl_destroy");

	   exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

// places the value of maxSize into shared memory
int* placeMaxSize(int maxSize, int *commid, key_t mkey){
	int *comm;

   // request memory that can hold maxSize (int)
   // if successful it returns a non-zero int
   if((*commid = shmget(mkey, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
	   // Call an error if the id is negative
	   die("shmget_placeSize");

	// attaches the requested memory to the address space of the server
	if((comm = shmat(*commid, NULL, 0)) == (void*) -1)
		// Call an error if the return is -1
		die("shmat_placeSize");

	// Set the value in shared memory to be maxSize
	*comm = maxSize;
	return comm;
}
