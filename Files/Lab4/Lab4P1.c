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
	int TARGET_FOUND_TAG = 0, TARGET_NOT_FOUND_TAG = 1;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	if(myRank == MASTER){ // Master
		while(1){

			int toFind = 0;
			// Get value to look for from user
			do{
				printf("Please enter an integer between 0 and %i\n", NUM_ITEMS);
			}while(scanf("%i", &toFind) != 1 || toFind >= NUM_ITEMS);

			// Notify slaves of value
			MPI_Bcast(&toFind, 1, MPI_INT, 0, MPI_COMM_WORLD);

			if(toFind < 0)
				break;

			// Start checking inbox
			int data[2], messageWaiting, totalRead = 0;
			while(totalRead < numProc-1){
				// Check if there is a success message
				MPI_Iprobe(MPI_ANY_SOURCE, TARGET_FOUND_TAG, MPI_COMM_WORLD, &messageWaiting, &status);
				if(messageWaiting){
					MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, TARGET_FOUND_TAG, MPI_COMM_WORLD, &status);
					printf("The number was found by task# %i\n", data[0]);
					//printf("The number was found at index# %i\n", data[1]);
					totalRead++;
				}
				// Check if there is a failure message
				MPI_Iprobe(MPI_ANY_SOURCE, TARGET_NOT_FOUND_TAG, MPI_COMM_WORLD, &messageWaiting, &status);
				if(messageWaiting){
					MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, TARGET_NOT_FOUND_TAG, MPI_COMM_WORLD, &status);
					totalRead++;
				}
			}

		}
	}else{ // Slaves

			// find the number of items for this process
			int numItems = NUM_ITEMS/(numProc-1);
			// get the analysis domain of this process
			int minElement = (myRank-1)*(numItems);


			// generate the table
			int* table = (int*) malloc(numItems * sizeof(int));
			// populate the table
			int i;
			int count = minElement;
			for(i = 0; i < numItems; i++){
				table[i] = count++;
			}

			int toFind, success, toSend[2];

		while(1){

			// Get value to look for from master
			MPI_Bcast(&toFind, 1, MPI_INT, 0, MPI_COMM_WORLD);

			// Check for termination
			if(toFind < 0)
				break;

			// Didn't terminate, iterate over table looking for toFind
			success = 0;
			for(i=0; i<numItems; i++){
				if(table[i]==toFind){
					// Target was found
					success = 1;
					break;
				}
			}

			// Package up data to send
			toSend[0] = myRank;
			toSend[1] = i;
			if(success){ // Send success tag
				MPI_Send(toSend, 1, MPI_INT, MASTER, TARGET_FOUND_TAG, MPI_COMM_WORLD);
				//printf("Sending from task %i with index %i\n", toSend[0], toSend[1]);
			}
			else // Send not found tag
				MPI_Send(toSend, 1, MPI_INT, MASTER, TARGET_NOT_FOUND_TAG, MPI_COMM_WORLD);

		}
	}
	
	/* shut down MPI */
	MPI_Finalize();

	return EXIT_SUCCESS;
}
