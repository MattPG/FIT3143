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

// Declare functions
void die(char* s);
void getAB(FILE *inputFile, double *A, int Arows, int Acols, double *B, int Brows, int Bcols);
void getABDim(FILE *inputFile, int* Arows, int* Acols, int* Brows, int* Bcols);
void matrixMultiply(double* myA, int rowsEach, double* B, int Bcols, double* myR, int Acols, int start);
void printMatrices(double *A, int Arows, int Acols, double *B, int Brows, int Bcols, double *R);


int main(int argc, char* argv[]){
	// Declare matrices to hold data
	double *A, *B, *R, *myA, *myR;
	int myRank, numProc, rowsEach, remainder;
	int Arows, Acols, Brows, Bcols, sizeA[2], sizeB[2];
	int MASTER = 0;

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
	if (numProc < 1 ) {
	  printf("Need at least one MPI task. Quitting...\n");
	  MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	  exit(EXIT_FAILURE);
	 }
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	// Master gets matrices
	if(myRank == MASTER){
		char *fileName = "matrices.txt";
		FILE *inputFile;

		printf("Master reading in matrix data...");
		// open up matrix file for reading
		if(!(inputFile = fopen(fileName,"r")))
			die("fopen");

		// Get dimensions of A and B
		getABDim(inputFile, &Arows, &Acols, &Brows, &Bcols);

		// Verify multiplicity
		if(Acols != Brows)
			die("Matrices' sizes don't allow for multiplication.\n");

		// Create A and B using dimensions
		MPI_Alloc_mem(Arows*Acols*sizeof(double), MPI_INFO_NULL, &A);
		MPI_Alloc_mem(Brows*Bcols*sizeof(double), MPI_INFO_NULL, &B);
		MPI_Alloc_mem(Arows*Bcols*sizeof(double), MPI_INFO_NULL, &R);

		// Read data into A and B
		getAB(inputFile, A, Arows, Acols, B, Brows, Bcols);



		// close matrix file from reading
		fclose(inputFile);
		printf("success\n");

		// define sizeA
		sizeA[0] = Arows;
		sizeA[1] = Acols;

		// define sizeB
		sizeB[0] = Brows;
		sizeB[1] = Bcols;
	}

	// BroadCast matrices' dimensions
    MPI_Bcast(sizeA, 2, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(sizeB, 2, MPI_INT, MASTER, MPI_COMM_WORLD);

   if(myRank != MASTER){
	    // Define local values of dimensions
		Arows = sizeA[0];
		Acols = sizeA[1];
		Brows = sizeB[0];
		Bcols = sizeB[1];

		// Create variable for B and A for referencing using dimensions
		A = (double*) NULL;
		MPI_Alloc_mem(Brows*Bcols*sizeof(double), MPI_INFO_NULL, &B);
		R = (double*) NULL;
   }

	// Number of Rows computed by each slave
	rowsEach = Arows/numProc;

	// Number of extra Rows computed by master
	remainder = Arows%numProc;

	// Local variable for storing smallA
	MPI_Alloc_mem(rowsEach*Acols*sizeof(double), MPI_INFO_NULL, &myA);

	// Broadcast matrix B
    if(MPI_Bcast(B, Brows*Bcols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD)!=MPI_SUCCESS)
    	die("MPI_Bcast");

    // Send unique rows of Matrix A
    if(MPI_Scatter(A, rowsEach*Acols, MPI_DOUBLE, myA, rowsEach*Acols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD)!=MPI_SUCCESS)
    		die("MPI_Scatter");

    // Local variable for the result
	MPI_Alloc_mem(rowsEach*Bcols*sizeof(double), MPI_INFO_NULL, &myR);

    // Multiply myA * B = myR
    matrixMultiply(myA, rowsEach, B, Bcols, myR, Acols, 0);

    // Return local results to Master
    if(MPI_Gather(myR, rowsEach*Bcols, MPI_DOUBLE, R, rowsEach*Bcols, MPI_DOUBLE, MASTER, MPI_COMM_WORLD)!=MPI_SUCCESS)
    	die("MPI_Gather");

    if(myRank == MASTER){
    	// Master computes the remainder rows
    	int start = Arows-remainder;
    	matrixMultiply(A, Arows, B, Bcols, R, Acols, start);

    	// Print matrix A, B and R
		printMatrices(A,Arows,Acols,B,Brows,Bcols,R);

	    // Free the allocated arrays
	    free(A);
	    free(B);
	    free(R);
	    free(myA);
	    free(myR);
    }

	/* shut down MPI */
	MPI_Finalize();

	return EXIT_SUCCESS;
}


/* function to read in Matrix A and Matrix B */
void getAB(FILE *inputFile, double *A, int Arows, int Acols, double *B, int Brows, int Bcols) {
	int i,j;

	// Read in matrix A
	for (i=0; i<Arows; i++)
		for (j=0; j<Acols; j++)
			if(fscanf(inputFile, "%lf", &(A[i*Acols +j])) != 1)
				die("fscanf Matrix A values");

	// Read in matrix B
	for (i=0; i<Brows; i++)
		for (j=0; j<Bcols; j++)
			if(fscanf(inputFile, "%lf", &B[i*Bcols +j]) != 1)
				die("fscanf Matrix B values");
}

// Function gets the dimensions of matrix A and matrix B
void getABDim(FILE *inputFile, int* Arows, int* Acols, int* Brows, int* Bcols){
	// Get matrices' dimensions
	if(fscanf(inputFile, "%d %d %d %d",Arows,Acols,Brows,Bcols) != 4)
		die("fscanf matrix dimensions");
}

// multiplies Matrix myA with matrix B and stores the results in myR
void matrixMultiply(double* myA, int rowsEach, double* B, int Bcols, double* myR, int Acols, int start){
int i, j, k;
for(i=start; i<rowsEach; i++)
	for(j=0; j<Bcols; j++)
		for(k=0; k<Acols; k++)
			myR[i*Bcols+j]+= myA[i*Acols+k]*B[k*Bcols+j];
}

/* function to print matrices A,B,C */
void printMatrices(double *A, int Arows, int Acols, double *B, int Brows, int Bcols, double *R){
	int i,j;

	printf("\nMatrix A (%d x %d)\n", Arows, Acols);
	for (i=0; i<Arows; i++) {
		for (j=0; j<Acols; j++)
			printf("%12lf ",A[i*Acols +j]);
		printf("\n");
	}

	printf("\nMatrix B (%d x %d)\n", Brows, Bcols);
	for (i=0; i<Brows; i++) {
		for (j=0; j<Bcols; j++)
			printf("%12lf ",B[i*Bcols +j]);
		printf("\n");
	}


	printf("\nMatrix R = Matrix A * Matrix B (%d x %d)\n", Arows, Bcols);
	for (i=0; i<Arows; i++) {
		for (j=0; j<Bcols; j++)
			printf("%12lf ",R[i*Bcols +j]);
		printf("\n");
	}

	printf("\n");

}

// function to kill program in case of error
void die(char* s) {
	perror(s);	// error message
	MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}
