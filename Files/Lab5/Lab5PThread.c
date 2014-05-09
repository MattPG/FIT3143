// MATTHEW GREENWOOD 23424303
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// HEADER
// method a pthread calls to parse the string
void printMatrices(double *A, int Arows, int Acols, double *B, int Brows, int Bcols, double *R);
void matrixMultiply(double* myA, int rowsEach, double* B, int Bcols, double* myR, int Acols, int start);
void getAB(FILE *inputFile, double *A, int Arows, int Acols, double *B, int Brows, int Bcols);
void getABDim(FILE *inputFile, int* Arows, int* Acols, int* Brows, int* Bcols);
static void* threadStart(void* input);
void die(char* s);

// Declare global counter and string
char *string;
int fileSize, spacing;

// Declare global matrices to hold data
double *A, *B, *R;
// Variables for the dimensions of the matrices
int Arows, Acols, Brows, Bcols, rowsEach;

int main(void) {
	int remainder;

	char *fileName = "matrices.txt";
	FILE *inputFile;

	printf("Reading in matrix data...");
	// open up matrix file for reading
	if(!(inputFile = fopen(fileName,"r")))
		die("fopen");

	// Get dimensions of A and B
	getABDim(inputFile, &Arows, &Acols, &Brows, &Bcols);

	// Verify multiplicity
	if(Acols != Brows)
		die("Matrices' sizes don't allow for multiplication.\n");

	// Create A and B using dimensions
	A = calloc(Arows*Acols, sizeof(double));
	B = calloc(Brows*Bcols, sizeof(double));
	R = calloc(Arows*Bcols, sizeof(double));

	// Read data into A and B
	getAB(inputFile, A, Arows, Acols, B, Brows, Bcols);

	// close matrix file from reading
	fclose(inputFile);
	printf("success\n");

	// set one thread per row
	int NUM_THREADS = Arows;

	// Number of Rows computed by each slave
	rowsEach = Arows/NUM_THREADS;

	// Number of extra Rows computed by master
	remainder = Arows%NUM_THREADS;

   // create threads
   printf("Creating threads...");
   int i, threadID[NUM_THREADS];
   pthread_t threads[NUM_THREADS];

   for(i=0; i < NUM_THREADS; i++){
	 if (pthread_create(&(threads[i]), NULL, &threadStart, (void*) &threadID[i]))
	   die("pthread_create");
	}
   printf("Success\n");


   if(remainder >0){
		// Compute remainder
		int start = Arows-remainder;
		matrixMultiply(A, Arows, B, Bcols, R, Acols, start);
   }

   // wait for all threads to finish
   printf("Waiting for threads to finish...");
   for(i = 0; i<NUM_THREADS; i++)
	   pthread_join(threads[i], NULL);
   printf("Success\n");

   // Display the matrices
   printMatrices(A,Arows,Acols,B,Brows,Bcols,R);

   // Free the allocated arrays
   free(A);
   free(B);
   free(R);

   exit(EXIT_SUCCESS);
}

// Driver for threads
static void* threadStart(void* input){
	int start, *threadID;
	threadID = (int*) input;

	start = *threadID*rowsEach;
	matrixMultiply(A, rowsEach, B, Bcols, R, Acols, start);
	return (void*) &A;
}

// multiplies Matrix myA with matrix B and stores the results in myR
void matrixMultiply(double* myA, int rowsEach, double* myB, int Bcols, double* myR, int Acols, int start){
int i, j, k;
for(i=start; i<rowsEach; i++)
	for(j=0; j<Bcols; j++)
		for(k=0; k<Acols; k++)
			myR[i*Bcols+j]+= myA[i*Acols+k]*myB[k*Bcols+j];
}

// die prints out a descriptive error message then quits the program
void die(char* s) {
    perror(s);	// error message
    exit(EXIT_FAILURE);	// exit call with unsuccessful completion of program
}

/* function to print matrices A,B,C */
void printMatrices(double *A, int Arows, int Acols, double *B, int Brows, int Bcols, double *R){
	int i,j;

	printf("\nMatrix A (%d x %d)\n", Arows, Acols);
	for (i=0; i<Arows; i++) {
		for (j=0; j<Acols; j++)
			printf("%5.0lf ",A[i*Acols +j]);
		printf("\n");
	}

	printf("\nMatrix B (%d x %d)\n", Brows, Bcols);
	for (i=0; i<Brows; i++) {
		for (j=0; j<Bcols; j++)
			printf("%5.0lf ",B[i*Bcols +j]);
		printf("\n");
	}


	printf("\nMatrix R = Matrix A * Matrix B (%d x %d)\n", Arows, Bcols);
	for (i=0; i<Arows; i++) {
		for (j=0; j<Bcols; j++)
			printf("%5.0lf ",R[i*Bcols +j]);
		printf("\n");
	}

	printf("\n");

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