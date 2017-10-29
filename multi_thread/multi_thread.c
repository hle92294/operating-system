#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
 
#define SIZE 2000  // matrices size
int num_thrd;     // number of threads
int **m_a;        // matrix A
int **m_b;        // matrix B
int **m_c;        // matrix C 
char user_in; 

/*Initialize matrix: 
  1. Allocate memory each element of each matrix */
void create_matrices(){
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
        /* Assign random value to each matrix's element*/
         m_a[i][j] = abs(rand()%10);  
         m_b[i][j] = abs(rand()%10);
      }
   }
}
/* 
Computed matrix 
May take a long time to display the result
*/
void display_result() {
  int i, j;
  for(i = 0; i < SIZE; i++) {
    for(j = 0; j < SIZE; j++) {
      printf("%d ", m_c[i][j]);
      }
    printf("\n");
  }
}
/* 
Thread function take in an argument 
  and assigning matrix's row 
*/
void* matrix_multi(void* data)
{
  int id = (int)data; 
  int start = (id * SIZE)/num_thrd; 
  int end = ((id+1) * SIZE)/num_thrd; 
  int i,j,k;
 
  printf("Procressing thread %d \t from row %d to %d\n", id, start, end-1);
  for (i = start; i < end; i++)
  {  
    for (j = 0; j < SIZE; j++)
    {
      m_c[i][j] = 0;
      for ( k = 0; k < SIZE; k++)
        m_c[i][j] += m_a[i][k]*m_b[k][j];
    }
  }
  printf("Thread %d finished\n", id);
  return 0;
}
 
int main(int argc, char* argv[])
{
  pthread_t* thread; 
  int i;
  if (argc!=2)
  {
    printf("Usage: %s <number_of_threads>\n",argv[0]);
    exit(-1);
  }
  num_thrd = atoi(argv[1]);
  create_matrices();
  thread = (pthread_t*) malloc(num_thrd*sizeof(pthread_t));

  /* Start the clock*/
  struct timespec begin, end;
  double elapsed;
  clock_gettime(CLOCK_MONOTONIC, &begin);
  clock_t start = clock(), diff;
  int start_time = time(NULL);

  /*Creating p-threads */
  for (i = 1; i < num_thrd; i++)
  {
    if (pthread_create (&thread[i], NULL, matrix_multi, (void*)i) != 0 )
    {
      perror("Thread can not be create!");
      free(thread);
      exit(-1);
    }
  }
  matrix_multi(0);

  for (i = 1; i < num_thrd; i++) {
    pthread_join (thread[i], NULL);
  }
 /* End the clock */
  clock_gettime(CLOCK_MONOTONIC,&end);
  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec)/1000000000.0;
  diff = clock() - start; 
  int end_time = time(NULL);
  int msec = diff * 1000 / CLOCKS_PER_SEC;

  /*Output info:
    1. CPU Time 
    2. Execution Time
    3. Runtime */
  printf("Matrix mutiplication process completed!\n");
  printf("------------------------------------------\n");
  printf("Total CPU-Time: %d seconds %d milliseconds\n", msec/1000, msec%1000);
  printf("Execution time: %f\n", elapsed);
  printf("Runtime: %ds\n", end_time - start_time);
  printf("Would you like to see the result matrix? Enter Y or N: ");
  scanf(" %c", &user_in); 
  if (user_in == 'Y') {
    display_result();
  } 
  /* Deallocate reserved memory */
  printf("Stop successfully! \n");
  for (int i=0; i<SIZE; i++) {
          free(m_a[i]);
          free(m_b[i]);
          free(m_c[i]);
  }
  free(m_a);
  free(m_b);
  free(m_c);
  free(thread);
 
  return 0;
}