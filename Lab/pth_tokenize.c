//gcc -g -Wall -o pth_tokenize pth_tokenize.c -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX = 1000;

int thread_count;
sem_t* sems;

void Usage(char* prog_name);
void *Tokenize(void* rank);  /* Thread function */

int main(int argc, char* argv[]) {
   long        thread;
   pthread_t* thread_handles; 

   if (argc != 2)
      Usage(argv[0]);
   thread_count = atoi(argv[1]);

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
   sems = (sem_t*) malloc(thread_count*sizeof(sem_t));
   sem_init(&sems[0], 0, 1);
   for (thread = 1; thread < thread_count; thread++)
      sem_init(&sems[thread], 0, 0);

   printf("Enter text\n");
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL,
          Tokenize, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }

   for (thread=0; thread < thread_count; thread++)
      sem_destroy(&sems[thread]);

   free(sems);
   free(thread_handles);
   return 0;
}  /* main */



void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   exit(0);
} 


void *Tokenize(void* rank) {
   long my_rank = (long) rank;
   int count;
   int next = (my_rank + 1) % thread_count;
   char *fg_rv;
   char my_line[MAX];
   char *my_string;

   sem_wait(&sems[my_rank]);  
   fg_rv = fgets(my_line, MAX, stdin);
   sem_post(&sems[next]);  
   while (fg_rv != NULL) {
      printf("Thread %ld > my line = %s", my_rank, my_line);

      count = 0; 
      my_string = strtok(my_line, " \t\n");
      while ( my_string != NULL ) {
         count++;
         printf("Thread %ld > string %d = %s\n", my_rank, count, my_string);
         my_string = strtok(NULL, " \t\n");
      } 
      if (my_line != NULL) printf("Thread %ld > After tokenizing, my_line = %s\n",
        my_rank, my_line);

      sem_wait(&sems[my_rank]); 
      fg_rv = fgets(my_line, MAX, stdin);
      sem_post(&sems[next]);  
   }

   return NULL;
}  
