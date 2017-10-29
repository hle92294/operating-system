#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 2000

int main() {
  printf("Program successfully start, please wait...\n");
	int result = 0;
    int **m_a = (int **)malloc(SIZE * sizeof(int *));
    int **m_b = (int **)malloc(SIZE * sizeof(int *));
    int **m_c = (int **)malloc(SIZE * sizeof(int *));
    
    for (int i=0; i<SIZE; i++) {
      m_a[i] = (int *)malloc(SIZE * sizeof(int));
    	m_b[i] = (int *)malloc(SIZE * sizeof(int));
    	m_c[i] = (int *)malloc(SIZE * sizeof(int));
    }

    for (int i = 0; i <  SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        /* Assign value */
       m_a[i][j] = abs(rand()%10);  
       m_b[i][j] = abs(rand()%10);
      }
    }

  /* Start time 
    Perform matrix multiplication*/
  int start_time = time(NULL);
 	clock_t start = clock(), diff; 
    for (int i = 0; i <  SIZE; i++) {
      	for (int j = 0; j < SIZE; j++) {
      		for (int k = 0; k < SIZE; k++) {
       			result += (m_a[i][k] * m_b[k][j]);
      		}
      	m_c[i][j] = result;
        result = 0; 
    	}
    }
  /* End time and display the result*/
  int end_time = time(NULL);
  diff = clock() - start; 
	int msec = diff * 1000 / CLOCKS_PER_SEC; // get clock in miliseconds
	char user_in; 
  printf("------------------------------------------\n");
 	printf("Total CPU-Time: %d seconds %d milliseconds\n", msec/1000, msec%1000);
  printf("Total Runtime: %ds\n", end_time - start_time);
  printf("Would you like to see the result matrix? Enter Y or N: ");
  scanf(" %c", &user_in); 
  if (user_in == 'Y') {
    for(int i = 0; i < SIZE; i++) {
      for(int j = 0; j < SIZE; j++) {
        printf("%d ", m_c[i][j]);
        }
      printf("\n");
    }
  } 
 	for (int i = 0; i <  SIZE; i++) {
  	      free(m_a[i]);
          free(m_b[i]);
          free(m_c[i]);
    }
    free(m_a);
    free(m_b);
	  free(m_c);
   return 0;
}





