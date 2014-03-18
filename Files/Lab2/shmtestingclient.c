//SHMClient.C
/*
 * This program prints out whatever exercise 2 places into memory
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>


// die prints out a descriptive error message then quits the program
void die(char *s)
{
    perror(s);	// print error
    sleep(1);	// pause the thread for 1 second
    exit(1);	// exit program with unsuccessful completion
}

// initial function call
int main() {
	// declare required variables and pointers
    int shmid;
    key_t key = 65631;
    char *shm, *s;
    int MAXSIZE = 1163;
    // request memory of size MAXSIZE with key above and place id in shmid
    if ((shmid = shmget(key, MAXSIZE, 0666)) < 0)
    	// if id is negative then throw error
        die("shmget");

    // attach memory to the address space of this program
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    	// throw error if shmat returns -1
        die("shmat");

    //Now read what the server put in the memory.
    for (s = shm; *s != '\0'; s++)
        putchar(*s);
    putchar('\n');

    /*
     *Change the first character of the
     *segment to '*', indicating we have read
     *the segment.
     */
    *shm = '*';

    // exit program with successful completion
    exit(0);
}
