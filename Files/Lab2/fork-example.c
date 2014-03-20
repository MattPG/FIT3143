// MATTHEW GREENWOOD 23424303
/*
 *  This program creates a new thread
 * 	using the fork() function. The child
 * 	increments previously defined variables
 * 	whilst the parent leaves them unchanged.
 * 	The results are printed by both threads.
 */

#include <stdio.h>

// Required by fork routine
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>   // Declaration for exit()

// declare global int and intialise to 2
int globalVariable = 2;

// initial function call
int main() {
	// declarations of required variable and pointers
   char*  sIdentifier;
   int    iStackVariable = 20;

   // call fork function to split program and it the return process ID
   pid_t pID = fork();
   if (pID == 0) {	// child

      // Code only executed by child process
	  sIdentifier = "Child Process: "; //give the child an identifier
      globalVariable++; // increment global variable
      iStackVariable++;	// increment stack variable

    } else if (pID < 0) {	// failed to fork

    	printf("Failed to fork\n"); // print error
        exit(1); // throw exception

    } else {	// parent

      // Code only executed by parent process
      sIdentifier = "Parent Process:"; //give the parent an identifier

    }

    // Code executed by both parent and child.

    printf("%s   Global variable: %i   Stack variable: %i\n", sIdentifier, globalVariable, iStackVariable);

    return 0;
}
