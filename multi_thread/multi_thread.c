#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10
#define SIZE 20

int **m_a; 
int **m_b; 
int **m_c; 
 
struct v {
   int i; /* row */
   int j; /* column */
};

void *runner(void *param); /* the thread */

int main(int argc, char *argv[]) {
   m_a = (int **)malloc(SIZE * sizeof(int *));
   m_b = (int **)malloc(SIZE * sizeof(int *));
   m_c = (int **)malloc(SIZE * sizeof(int *));
   for (int i=0; i<SIZE; i++) {
      m_a[i] = (int *)malloc(SIZE * sizeof(int));
      m_b[i] = (int *)malloc(SIZE * sizeof(int));
      m_c[i] = (int *)malloc(SIZE * sizeof(int));
   }

   for (int i = 0; i <  SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        /* Assign value */
         m_a[i][j] = 5;  
         m_b[i][j] = 5;
      }
   }

   //int i,j, count = 0;
   int count = 0;
   for(int i = 0; i < SIZE; i++) {
      for(int j = 0; j < SIZE; j++) {
         //Assign a row and column for each thread
         struct v *data = (struct v *) malloc(sizeof(struct v));
         data->i = i;
         data->j = j;
         /* Now create the thread passing it data as a parameter */
         pthread_t tid;       //Thread ID
         pthread_attr_t attr; //Set of thread attributes
         //Get the default attributes
         pthread_attr_init(&attr);
         //Create the thread
         pthread_create(&tid,&attr,runner,data);
         //Make sure the parent waits for all thread to complete
         pthread_join(tid, NULL);
         count++;
      }
   }

   //Print out the resulting matrix
   for(int i = 0; i < SIZE; i++) {
      for(int j = 0; j < SIZE; j++) {
         printf("%d ", m_c[i][j]);
      }
      printf("\n");
   }
}

//The thread will begin control in this function
void *runner(void *param) {
   struct v *data = param; // the structure that holds our data
   int n, sum = 0; //the counter and sum

   //Row multiplied by column
   for(n = 0; n< SIZE; n++){
      sum += m_a[data->i][n] * m_b[n][data->j];
   }
   //assign the sum to its coordinate
   m_c[data->i][data->j] = sum;

   //Exit the thread
   pthread_exit(0);
}
