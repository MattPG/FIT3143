#include <sys/shm.h> //shm
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <errno.h> //perror

// placeSize puts an int into common shared memory
int* placeMaxSize(int maxSize, int *shmid);
// die prints out a descriptive error message then quits the program
void die(char* s);
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

int main(void) {
	// declare required variables and pointers
	char *fileName = "input_text.txt";
	char *shm;
	int *comm, *counter;
	int fileSize, readSize, maxSize, shmid, charSize, commid, counterid;
	key_t key, ckey;
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
	   comm = placeMaxSize(maxSize, &commid);

	   // generate ID for key
	   key = 5343;

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

	   // generate value for ckey
	   ckey = 15;

	   // create the shared memory counter
	   if((counterid = shmget(ckey, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
		   die("shmget_counterid");

	   // attach counter
	   if((counter = shmat(counterid, NULL, 0)) == (void*)-1)
			   die("shmat_counter");

	   // initialise counter to zero
	   *counter = 0;

	   // wait until other programs finish
	   while(*shm != '$'){
		   sleep(1);
		   printf("Waiting for other programs!\n");
	   }

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

	   // mark countter for destruction
	   if(shmctl(counterid, IPC_RMID, 0)==-1)
		   die("shmctl_counter");

	   exit(EXIT_SUCCESS);
	} else	die("fopen");

	exit(EXIT_FAILURE);
}

int* placeMaxSize(int maxSize, int *commid){
	key_t key = 10;
	int *comm;

   // request memory that can hold maxSize (int)
   // if successful it returns a non-zero int
   if((*commid = shmget(key, sizeof(int), IPC_CREAT | IPC_EXCL | 0666))<0)
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