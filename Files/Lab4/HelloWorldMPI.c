/*
 ============================================================================
 Name        : HelloWorldMPI.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C 
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[]){
	int  myRank; /* rank of process */
	int  numProc;       /* number of processes */
	MPI_Status status ;   /* return status for receive */
	
	int NUM_ITEMS = 10000; // number of items to process
	int MASTER = 0;
	int TARGET_FOUND_TAG = 0;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	if (myRank == MASTER){ // Master
		while(1){

			int toFind;
			// Get value to look for from user
			printf("Please enter an integer between 0 and %i\n", NUM_ITEMS);
			scanf("%i", &toFind);

			// Notify slaves of value
			MPI_Bcast(&toFind, 1, MPI_INT, 0, MPI_COMM_WORLD);

			if(toFind < 0)
				break;

			int slaveID;
			MPI_Recv(&slaveID, 1, MPI_INT, MPI_ANY_SOURCE, TARGET_FOUND_TAG, MPI_COMM_WORLD, &status);

			printf("The number was found by task#%i!!!!\n", slaveID);
		}
	}
	else{ // Slaves

			// find the number of items for this process
			int numItems = NUM_ITEMS/(numProc-1);
			// get the analysis domain of this process
			int maxElement = (myRank)*(numItems);
			int minElement = (myRank-1)*(numItems);


			// generate the table
			int* table = (int*) malloc(numItems * sizeof(int));
			// populate the table
			int i;
			int count = minElement;
			for(i = 0; i < numItems; i++){
				table[i] = count++;
			}

			int toFind, toSend;

		while(1){

			// Get value to look for from master
			MPI_Bcast(&toFind, 1, MPI_INT, 0, MPI_COMM_WORLD);

			if(toFind < 0)
				break;

			toSend = 0;
			// parse the array for the value to find

			if(toFind < maxElement && toFind >= minElement)
				// Send success tag
				toSend = 1;


			MPI_Gather();
		}
	}
	
	/* shut down MPI */
	MPI_Finalize();

	return EXIT_SUCCESS;
}
