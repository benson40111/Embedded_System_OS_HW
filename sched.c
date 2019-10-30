#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

#define NLOOP_FOR_ESTIMATION 1000000000UL
#define NSECS_PER_MSEC 1000000UL
#define NSECS_PER_SEC 1000000000UL

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

double keep = 0.0;

static double TimeSpecToSeconds(struct timespec* ts)
{
    return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000.0;
}

static inline void load(unsigned long nloop)
{
        unsigned long i;
        for (i = 0; i < nloop; i++)
                ;
}

static inline long diff_nsec(struct timespec before, struct timespec after)
{
        return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
                - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));

}

static unsigned long loops_per_msec()
{
        struct timespec before, after;
        clock_gettime(CLOCK_MONOTONIC, &before);

        unsigned long i;
        for (i = 0; i < NLOOP_FOR_ESTIMATION; i++)
		;

        clock_gettime(CLOCK_MONOTONIC, &after);

	int ret;
        return  NLOOP_FOR_ESTIMATION * NSECS_PER_MSEC / diff_nsec(before, after);
}

// 子執行緒函數
void *child(void* id) {
	pthread_mutex_lock(&mutex1);
	printf("Process %d(%d) was created...\n", id, pthread_self());
	pthread_mutex_unlock(&mutex1);

	pthread_mutex_lock(&mutex);

	struct timespec start;
	struct timespec end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for (int i = 0 ; i < 3 ; i++)
		printf("%d(%d) is running...\n", id, pthread_self());
	clock_gettime(CLOCK_MONOTONIC, &end);
	keep = TimeSpecToSeconds(&end) - TimeSpecToSeconds(&start);
	printf("Total time: %.4lf msec\n", keep);
	pthread_mutex_unlock(&mutex);

}

int main() {
	pthread_t t[3]; 

	for (int i = 0; i < 3; i++) {
		memset(&t[i], 0, sizeof(t[i]));
		pthread_create(&t[i], NULL, child, i);
	}

	for (int i = 0; i < 2; i++)
		pthread_join(t[i], NULL);
	return 0;
} 