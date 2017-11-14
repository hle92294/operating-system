
/* Include dependencies */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <semaphore.h>
#include <stdint.h>
//#include <windows.h>

/* Definations */
#define CPU_THREAD 	8
#define IO_THREAD 	4
#define SUB_THREAD 	4
#define MAX 		5


/* Semaphore CPU, IO and Job locks */

sem_t cpu_lock;
sem_t io_lock;
sem_t job_lock;
sem_t PC_lock;

/* Job definition */

typedef struct _job
{
    int job_id;
    int is_completed;
    int nr_jobs;
    int current_phase;
    int ** nr_phases;

} _job;

/* List Node Defination */

typedef struct Node
{
    _job* job;
    struct Node* next;
    
} node;

/* Queue definition */

typedef struct _queue
{
    node* head;
    node* tail;
    int queue_size;

} _queue;

/* Queue pointers */

_queue *cpu_process_handler;
_queue *io_handler;
_queue *job_submission;

/* Queue Setup to Empty */

void queue_setup(_queue * que)
{
    que->queue_size = 0;

}

/* Queue removal */

_job* remove_queue(_queue *que)
{
    if (que->queue_size==0)
    {
        return NULL;
    }
    _job* jb = que->head->job;
    node* curr = que->head;
    curr->job = NULL;

    que->head = que->head->next;
    que->queue_size--;
    free(curr);
    return jb;
}

/* Queue Insertion */

void insert_queue(_queue *que, _job *jb)
{
    node * curr = (node*)malloc(sizeof(node));
    curr->job = jb;
    curr->next = NULL;

    if (que->queue_size == 0)
    {
        que->head = curr;
        que->tail = curr;
    }
    else
    {
        que->tail->next = curr;
        que->tail = curr;
    }
    que->queue_size++;
}

/* Ready state function */

int ready(int curr_time)
{
    int duration = 0;
    struct timespec start;
    struct timespec finish;
    /* Get start and finish times */
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    /* Perform Calculation */
    while (duration <= curr_time)
    {
        clock_gettime(CLOCK_MONOTONIC, &finish);
        duration = finish.tv_sec - start.tv_sec;
    }
    return 1;
}

/* Enable Processes and Program Counter */

int enable = 0;
unsigned int PC = 1;


/* Job Insertion and creation */

void insert_job(_job *jb, int job_Id)
{
    srand(time(NULL));

    /* Define values and allocate memory */
    int phasetype;
    int PC = 2;
    jb->current_phase = 0;
    jb->is_completed = 0;
    jb->job_id = job_Id;
    jb->nr_jobs = rand()%4+2;

    jb->nr_phases = (int **)malloc(PC * sizeof(int *));

    /* Allocate memory for all Phases */
    for (int k=0; k < PC; k++)
    {
        jb->nr_phases[k] = (int *)malloc(jb->nr_jobs * sizeof(int));
    }

    /* For loop for Random values of Phases */
    for (int i=0; i < jb->nr_jobs; i++)
    {
        phasetype = rand()%PC;

        if (phasetype == 1)
        {
            jb->nr_phases[1][i] = 1;
            jb->nr_phases[0][i] = rand()% 4+7;
        }
        else
        {
            jb->nr_phases[1][i] = 0;
            jb->nr_phases[0][i] = rand()% 4+1;
        }
    }

    return;

}

/* Job removal */

void remove_job(_job *jb)
{
    printf("Removing job ... \n");
    free(jb->nr_phases[1]);
    free(jb->nr_phases[0]);
    free(jb->nr_phases);
    free(jb);

}

/* Create IO process */

void *create_io_process(void * arg)
{
    printf("I/O Inititalizing ...\n");
    int * io_thread = (int *)arg;

    /* While Program Enable is On */
    while (enable!=1)
    {

        if (io_handler->queue_size != 0)
        {

            sem_wait(&io_lock);

            _job *io = remove_queue(io_handler);

            if (io == NULL)
            {
                sem_post(&io_lock);
                continue;
            }
            printf("IO Job %d\n", (int)io_thread);

            sem_post(&io_lock);

            printf("Job NR Phase %d is finished -> %d\n", io->current_phase, (int)io_thread);

            if (io->current_phase == io->nr_jobs)
            {
                io->is_completed = 1;
                sem_wait(&job_lock);
                printf("Transferring job to JOB SUBMISSION queue: IO -> %d\n", (int)io_thread);
                insert_queue(job_submission, io);
                sem_post(&job_lock);
            }

            else if (io->nr_phases[1][io->current_phase] == 1)
            {
                sleep(io->nr_phases[0][io->current_phase]);
                io->current_phase++;
                sem_wait(&io_lock);
                printf("Transferring job to CPU queue : IO -> %d\n",(int)io_thread);
                insert_queue(io_handler,io);
                sem_post(&io_lock);

            }
            else
            {
                sem_wait(&cpu_lock);
                printf("Transferring job to CPU queue : IO -> %d\n", (int)io_thread);
                insert_queue(cpu_process_handler,io);
                sem_post(&cpu_lock);


            }
        }
    }
    printf("IO %d task is complete\n", (int)io_thread);
    pthread_exit(0);
}

/* Job submission */

void * job_submission_process(void* arg)
{
    int time_check;
    int* job_sub_thread = (int*)arg;

    /* While Program Enable is On */
    while(enable != 1)
    {

        _job* jb = (_job*)malloc(sizeof(_job));

        time_check = 0;
        sem_wait(&PC_lock);
        insert_job(jb, PC);
        PC++;
        sem_post(&PC_lock);

        sem_wait(&cpu_lock);
        printf("Transferring job to JOB SUBMISSION: %d\n", (int)job_sub_thread);
        insert_queue(cpu_process_handler, jb);

        sem_post(&cpu_lock);

        while (time_check!=1)
        {
            if(job_submission->queue_size != 0)
            {
                sem_wait(&job_lock);

                if (job_submission->queue_size != 0)
                {
                    sem_post(&job_lock);
                    continue;
                }
                printf("Job complete: %d\n", (int)job_sub_thread);
                _job *jb = remove_queue(job_submission);
                remove_job(jb);
                free(jb);

                sem_post(&job_lock);
            }

            time_check = ready(3);
        }
    }
    printf("Job thread is complete: -> %d\n", (int)job_sub_thread);

    pthread_exit(0);
}

/* CPU Process */

void * cpu_process(void * arg)
{
    int *  cpu_thread = (int*)arg;

    while (enable != 1)
    {
        if (cpu_process_handler->queue_size != 0)
        {
            sem_wait(&cpu_lock);

            _job* cpu = remove_queue(cpu_process_handler);
            if (cpu == NULL)
            {
                sem_post(&cpu_lock);
                continue;
            }
            printf("Transferring job to CPU: -> %d\n", (int)cpu_thread);

            sem_post(&cpu_lock);

            printf("Job Number: %d\n", cpu->job_id);
            printf("Phase: %d -> Thread: %d\n", cpu->current_phase, (int)cpu_thread);

            if (cpu->current_phase == cpu->nr_jobs)
            {
                cpu->is_completed = 1;

                sem_wait(&job_lock);
                printf("Transferring job %d to JOB SUBMISSION Queue: -> %d\n", cpu->job_id, (int)cpu_thread);
                insert_queue(job_submission, cpu);

                sem_post(&job_lock);
            }
            if (cpu->nr_phases[1][cpu->current_phase] == 0)
            {

                sleep(cpu->nr_phases[0][cpu->current_phase]);
                cpu->current_phase++;
                sem_wait(&cpu_lock);
                printf("Transferring job to CPU Queue: -> %d\n", (int)cpu_thread);
                insert_queue(cpu_process_handler,cpu);
                sem_post(&cpu_lock);

            }
            if (cpu->nr_phases[1][cpu->current_phase] == 1)
            {

                sem_wait(&io_lock);
                printf("Transferring job to IO Queue: -> %d\n", (int)cpu_thread);
                insert_queue(io_handler,cpu);
                sem_post(&io_lock);

            }

        }
    }

    printf("CPU task is complete: -> %d\n", (int)cpu_thread);

    pthread_exit(0);
}


/* Main */

int main()
{
    /* Timer */

    //clock_t begin = clock();

    /* Inititalize Semaphore Locks */

    sem_init(&cpu_lock, 0, 1);
    sem_init(&io_lock, 0, 1);
    sem_init(&job_lock, 0, 1);

    /* Memory Allocations for Queues */

    cpu_process_handler =(_queue*) malloc(sizeof(_queue));
    io_handler =(_queue*) malloc(sizeof(_queue));
    job_submission = (_queue*)malloc(sizeof(_queue));

    queue_setup(cpu_process_handler);
    queue_setup(io_handler);
    queue_setup(job_submission);
    srand(time(NULL));

    /* Make Program Enable off */
    enable = 0;
    int th;
    sem_init(&PC_lock, 0, 1);

    /* For loops for Pthread Creations */

    /* Pthreads for 4 Job Submissions */
    pthread_t job_sub[SUB_THREAD];
    for(int i = 0 ; i < SUB_THREAD; i++)
    {
        th = pthread_create(&job_sub[i], NULL, job_submission_process,(void *)i );
        assert(th == 0);
    }

    /* Pthreads for 8 CPU */
    pthread_t _cpu[CPU_THREAD];
    for (int i = 0; i < CPU_THREAD; i++)
    {
        th = pthread_create(&_cpu[i], NULL, cpu_process,(void *)i);
        assert(th == 0);
    }

    /* Pthreads for 4 I/O */
    pthread_t _io[IO_THREAD];
    for (int i=0; i<IO_THREAD; i++)
    {
        th = pthread_create(&_io[i], NULL, create_io_process,(void *)i);
        assert(th == 0);
    }

    /* Delay and Enable Processes */
    ready(20);
    enable = 1;

    /* For loops for Pthread Join */
    for (int i=0; i < SUB_THREAD; i++)
    {
        th = pthread_join(job_sub[i],NULL);
        assert(th == 0);
    }
    for (int i=0; i < CPU_THREAD; i++)
    {
        th = pthread_join(_cpu[i],NULL);
        assert(th == 0);
    }
    for (int i=0; i < IO_THREAD; i++)
    {
        th = pthread_join(_io[i],NULL);
        assert(th == 0);
    }

    //clock_t end = clock();
    //float duration = (float)(end - begin) / CLOCKS_PER_SEC;

    printf("\n*-------------------------------------*\n");

    printf("Number of Jobs = %d\n", PC-1);

    //printf("Run Time = %f\n", duration);

    printf("MULTITHREAD TASKS COMPLETE\n");

    printf("\n*-------------------------------------*\n");


    return 0;
}



