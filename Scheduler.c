
#include <string.h>                    /* for strerror()                */
#include <stdio.h>                     /* for printf                    */
#include <pthread.h>                   /* P-thread implementation       */
#include <signal.h>                    /* for interrupt handling        */
#include <stdlib.h>
#define MAX_THREADS       10           /* Number of child threads       */
#define SCHEDULE_INTERVAL  1           /* interval for clock interrupt  */
                                       /* in second(s)                  */

#define TRUE               1           /* TRUE label                    */
#define FALSE              0           /* FALSE label                   */

/* Prototype ---------------------------------------------------------- */
void * child_thread (void *arg);           /* the child thread body         */
int  find_next_thread(void);           /* find the next thread to run   */
void test_thread_list(void);           /* for test use only             */
void initialize_thread_list(void);     /* initialize the thread schedule*/


/* Global variables --------------------------------------------------- */
pthread_mutex_t mutex;                 /* mutex semaphore               */
pthread_cond_t  cond[MAX_THREADS];     /* condition variables           */
pthread_cond_t  condition;
int time_to_check;                     /* "time to check" flag          */
int loop_counter;                      /* loop counter                  */
int message_to_stop_recieved;
pthread_t parent_thread;           /* Thread ID of the parent thread    */
pthread_t pt[MAX_THREADS];         /* thread descriptor (for system use)*/

int thread_list[MAX_THREADS][4];       /* the thread list               */

unsigned int current_time;             /* current time                  */  
unsigned int exec_time;                /* next execution time           */

/* The interrupt handler for SIGALM interrupt ------------------------- */ 
void clock_interrupt_handler(void)
{
   int next_child_to_run;     // The thread # for the next child thread 
   int local_counter = 0;
   message_to_stop_recieved = FALSE;
   time_to_check = TRUE;   
  while((message_to_stop_recieved == FALSE) && (current_time > 0))
   {
      ++local_counter;
   }

   
   current_time++;   

   printf("scheduler started ++++++++++++++++++++++++++++++++++ \n");
   printf("current time: %d\n", current_time);   
 

   // Find the next child to run ----------------------------------
   next_child_to_run = find_next_thread();
 
   time_to_check = FALSE;
   pthread_cond_signal(&cond[next_child_to_run]);
   thread_list[next_child_to_run][3] = thread_list[next_child_to_run][3] -1;
} 


void main (void)
{
   int  i;                 // general-purpose loop counter
   time_to_check = FALSE;   // initialize the "time_to_check" flag
   loop_counter = 0;
   int ack_to_sch    = FALSE;  // the ACK to the scheduler flag
   int continue_more = TRUE;   // set "continue flag"
   pthread_t tid;
   signal(SIGALRM,clock_interrupt_handler);
   message_to_stop_recieved = FALSE;
  

   //init mutex
    pthread_mutex_init(&mutex,NULL);

    for(int i = 0;i<MAX_THREADS;i++)
    {
      //init condition variables
      pthread_cond_init(&cond[i],NULL);
    }
  

    pthread_cond_init(&condition,NULL);

   // initialzie the thread list ------------------------------------
   initialize_thread_list();
   test_thread_list(); 

   // initialzie the scheduler time ---------------------------------
   current_time = 0;  
   exec_time = 0;  

   // create ten threads --------------------------------------------
  pthread_t threads[MAX_THREADS];
  for(int i = 0; i < MAX_THREADS; i++)
  {
    pthread_create(&tid,NULL,child_thread,(void *)i);
  }
 
//Assign parent thread id
  parent_thread = pthread_self();
  
  // wait for 1 second ---------------------------------------------
   sleep(1);


   // Inifinite loop for the parent thread --------------------------
   while (continue_more == TRUE)
   { 
    
     continue_more = FALSE;
     for(int i = 0; i < MAX_THREADS; i++)
     {
       if(thread_list[i][3] > 0)
       {
          continue_more = TRUE;
       }
     }   
     if(continue_more == TRUE)
     {
       alarm(1);
       pause();
     }
      else
      {
        sleep(1);
      }
              
   } 

   // terminating myself --------------------------------------------
   printf("the parent thread is terminating ....\n");
}

/* child-thread definition --------------------------------------------- */
void *child_thread(void *arg)
{
    int index = (int)arg;
    int local_counter = 0;
    
    
    pthread_mutex_lock(&mutex);
    loop_counter++;
    pthread_cond_wait(&cond[index],&mutex);
    pthread_mutex_unlock(&mutex);


    printf("Thread %d has started ...\n", index);

    // the main infinite loop for a child thread -------------------
    while(1)
    {
       local_counter ++; 

      if(time_to_check == TRUE)
      {
      message_to_stop_recieved = TRUE;
      pthread_cond_signal(&condition);
      pthread_cond_wait(&cond[index],&mutex);
      }     

       // show my presence -----------------------------------------
       if ((local_counter % 10000000) == 0)
       {  printf("     Thread %d is running ...\n", index); }  
   }
}

/* find the next thread to run ----------------------------------------- */
int find_next_thread(void)
{
int next_thread = 0;
int smallestRemainingTime;
int firstMatchFound = FALSE;
int nothingSmallerFound = FALSE;

while(thread_list[next_thread][1] > current_time || thread_list[next_thread][3] == 0)
{
 next_thread++;

}

for(int i = 0; i < MAX_THREADS;i++)
{
  if(thread_list[i][1] <= current_time && thread_list[i][3] != 0)
  {
        if(thread_list[i][3] < thread_list[next_thread][3])
        {
          next_thread = i;
        }
  }
}

   return (next_thread);
}

/* initialize the thread list ------------------------------------------ */
void initialize_thread_list(void)
{
    // Thread #0 ----------------------------------------
    thread_list[0][0] = 0;     // thread ID
    thread_list[0][1] = 1;     // start time
    thread_list[0][2] = 15;    // total execution time
    thread_list[0][3] = 15;    // remaining time
    
    // Thread #1 ----------------------------------------
    thread_list[1][0] = 1;     // thread ID
    thread_list[1][1] = 5;     // start time
    thread_list[1][2] = 9;     // total execution time
    thread_list[1][3] = 9;     // remaining time
    
    // Thread #2 ----------------------------------------
    thread_list[2][0] = 2;     // thread ID
    thread_list[2][1] = 3;     // start time
    thread_list[2][2] = 10;    // total execution time
    thread_list[2][3] = 10;    // remaining time
    
    // Thread #3 ----------------------------------------
    thread_list[3][0] = 3;     // thread ID
    thread_list[3][1] = 5;     // start time
    thread_list[3][2] = 12;    // total execution time
    thread_list[3][3] = 12;    // remaining time
    
    // Thread #4 ----------------------------------------
    thread_list[4][0] = 4;     // thread ID
    thread_list[4][1] = 8;     // start time
    thread_list[4][2] = 3;     // total execution time
    thread_list[4][3] = 3;     // remaining time
    
    // Thread #5 ----------------------------------------
    thread_list[5][0] = 5;     // thread ID
    thread_list[5][1] = 20;    // start time
    thread_list[5][2] = 6;     // total execution time
    thread_list[5][3] = 6;     // remaining time
    
    // Thread #6 ----------------------------------------
    thread_list[6][0] = 6;     // thread ID
    thread_list[6][1] = 17;    // start time
    thread_list[6][2] = 9;     // total execution time
    thread_list[6][3] = 9;     // remaining time
    
    // Thread #7 ----------------------------------------
    thread_list[7][0] = 7;     // thread ID
    thread_list[7][1] = 11;    // start time
    thread_list[7][2] = 5;     // total execution time
    thread_list[7][3] = 5;     // remaining time
    
    // Thread #8 ----------------------------------------
    thread_list[8][0] = 8;     // thread ID
    thread_list[8][1] = 4;     // start time
    thread_list[8][2] = 10;    // total execution time
    thread_list[8][3] = 10;    // remaining time
    
    // Thread #9 ----------------------------------------
    thread_list[9][0] = 9;     // thread ID
    thread_list[9][1] = 6;     // start time
    thread_list[9][2] = 9;     // total execution time
    thread_list[9][3] = 9;     // remaining time
}


/* test thread list ------------------------------------------------- */
void test_thread_list(void)
{
   int  i;   // loop counter

   printf("thread ID, start time, execution time, remaining time\n");
   for (i = 0; i < MAX_THREADS; i++)
   {
      printf("%d,      %d,      %d,      %d\n", thread_list[i][0],
              thread_list[i][1], thread_list[i][2], thread_list[i][3]);
   }
   printf("\n\n"); 
}

