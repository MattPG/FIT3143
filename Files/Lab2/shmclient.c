// MATTHEW GREENWOOD 23424303
//SHMClient.C
/*
 * This program requests memory with the same size and key
 * as defined in shmserver.c and then prints out what the
 * server placed in memory at that address. So the server
 * knows this occurred, the first memory address is replaced
 * with an *.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE     27

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
    key_t key;
    char *shm, *s;

    // initialise the value of key (same as server)
    key = 5678;

    // request memory of size maxSize with key above and place id in shmid
    if ((shmid = shmget(key, maxSize, 0666)) < 0)
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
