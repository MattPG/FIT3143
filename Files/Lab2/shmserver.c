//SHMServer.C

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
# include <unistd.h>

#define MAXSIZE     27

// die prints out a descriptive error message then quits the program
void die(char *s) {
    perror(s);	// error message
    exit(1);	// exit call with unsuccessful completion of program
}

int main() {
	// declare required variables and pointers
    char c;
    int shmid;
    key_t key;
    char *shm, *s;

    // initialise the IPC key
    key = 5678;

    // shmget requests shared memory, if successful it returns a non-zero id
    if ((shmid = shmget(key, MAXSIZE, IPC_CREAT | 0666)) < 0)
    	// Call an error if the id is negative
        die("shmget");

    // attaches the requested memory to the address space of the server
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    	// Call an error if the return is -1
        die("shmat");

    /*
     *      * Put some things into the memory for the
     *        other process to read.
     *        */
    s = shm;

    for (c = 'a'; c <= 'z'; c++){
        *s++ = c;
    }


    /*
     * Wait until the other process
     * changes the first character of our memory
     * to '*', indicating that it has read what
     * we put there.
     */
    while (*shm != '*'){
        sleep(1)
    }

    // Kill program with successful completion
    exit(0);
}
