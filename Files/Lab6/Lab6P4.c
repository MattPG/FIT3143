// MATTHEW GREENWOOD 23424303
#include <stdio.h>
#include <stdlib.h>

#define MAX_ELEMENTS		10000
#define SLOWDOWN			10
#define CACHEHITS			1


int A[MAX_ELEMENTS][MAX_ELEMENTS];

int main(void) {

	int i, j, k;

	if(CACHEHITS == 1){
		printf("Cache hits");
		for(k = 0; k < SLOWDOWN; k++){
			for(i = 0; i < MAX_ELEMENTS; i++){
				for(j = 0; j < MAX_ELEMENTS; j++){
					A[i][j] = i*j;
				}
			}
		}
	} else{
		printf("No cache hits");
		for(k = 0; k < SLOWDOWN; k++){
			for(i = 0; i < MAX_ELEMENTS; i++){
				for(j = 0; j < MAX_ELEMENTS; j++){
					A[j][i] = i*j;
				}
			}
		}
	}

	exit(EXIT_SUCCESS);
}

