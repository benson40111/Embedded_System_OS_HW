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

static void child_fn(void* id, struct timespec *buf, int nrecord, unsigned long nloop_per_resol, struct timespec start)
{
		int i;
		for (int i = 0; i < nrecord; i++) {
				struct timespec ts;

				load(nloop_per_resol);
				clock_gettime(CLOCK_MONOTONIC, &ts);
				buf[i] = ts;
		}
		for (int i = 0; i < nrecord; i++)
				printf("%d(%d)\t%ld\n", pthread_self(), id, diff_nsec(start, buf[i]) / NSECS_PER_MSEC);

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
void *child(void* args) {
	struct timespec *logbuf;
	struct create_args *recv_args = (struct create_args*)args;
	int i = 0;
	recv_args.id = i;
	//logbuf = (*recv_args).logbuf;

	printf("Process %d was created...(%d)\n", i, pthread_self());
	pthread_mutex_lock(&mutex);

	unsigned long nloop_per_resol = loops_per_msec();

	clock_gettime(CLOCK_MONOTONIC, &start);
	child_fn(i, logbuf, 3, nloop_per_resol, start);

	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);

}

struct create_args {
	int id;
};


int main() {
		pthread_t t[2];	
		int nrecord = 3;
		struct timespec *logbuf = malloc(nrecord * sizeof(struct timespec));
		struct timespec start;


		for (int i = 0; i < 3; i++) {
				struct create_args thread_args;
				thread_args.id = i;
				memset(&t[i], 0, sizeof(t[i]));
				pthread_create(&t[i], NULL, child, &(thread_args)); // 建立子執行緒
		}

		for (int i = 0; i < 3; i++)
				pthread_join(t[i], NULL); // 等待子執行緒執行完成
		return 0;
}
