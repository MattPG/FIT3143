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
	int source;   /* rank of sender */
	int dest;     /* rank of receiver */
	int tag=0;    /* tag for messages */
	int NUM_ITEMS = 10000; // number of items to process
	char message[100];        /* storage for message */
	MPI_Status status ;   /* return status for receive */
	
	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	if (myRank !=0){ // Slaves
		// find the number of items for this process
		int numItems = NUM_ITEMS/(numProc-1);
		// get the analysis domain of this process
		int maxElement = (myRank)*(numItems);
		int minElement = (myRank-1)*(numItems);
		// generate the table
		int table[numItems];
		// fill the table
		int i;
		for(i = minElement; i < maxElement; i++){
			table[i] = i;
		}

		/* create message */
		sprintf(message, "Process %d! Computing between [%i,%i).", myRank,minElement,maxElement);
		dest = 0;
		/* use strlen+1 so that '\0' gets transmitted */
		MPI_Send(message, strlen(message)+1, MPI_CHAR,
		   dest, tag, MPI_COMM_WORLD);
	}
	else{ // Master
		printf("Please enter a number below %i.", NUM_ITEMS);

		// Search for incoming messages from each process
		for (source = 1; source < numProc; source++) {
			MPI_Recv(message, 100, MPI_CHAR, source, tag,
			      MPI_COMM_WORLD, &status);
			printf("%s\n",message);
		}
	}


	/* shut down MPI */
	MPI_Finalize(); 
	
	
	return EXIT_SUCCESS;
}
