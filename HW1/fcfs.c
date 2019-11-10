/*
Fork from github:https://github.com/satoru-takeuchi/linux-in-practice
Author:satoru-takeuchi
*/

#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int cpu_time = 0;
int total = 0;

void *child_fn(void *num)
{
    pthread_mutex_lock(&mutex1);
    printf("Process (%d)%d was created...\n", num, pthread_self());
    pthread_mutex_unlock(&mutex1);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < total; i++) {
        cpu_time++;
	    printf("Process (%d)%d\t%d\n", num, pthread_self(), cpu_time);
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);

}


int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <nproc> <total[ms]>\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    int nproc = atoi(argv[1]);
    total = atoi(argv[2]);

    if (nproc < 1) {
        fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
        exit(EXIT_FAILURE);
    }

    if (total < 1) {
        fprintf(stderr, "<total>(%d) should be >= 1\n", total);
        exit(EXIT_FAILURE);
    }
    
    fflush(stdout);

    int ncreated;
    pthread_t t[nproc];
    for (int i = 0 ; i < nproc ; i++)
        pthread_create(&t[i], NULL, child_fn, (void *) i);
    
    for (int i = 0 ; i < nproc ; i++)
        pthread_join(t[i], NULL);
    
}
