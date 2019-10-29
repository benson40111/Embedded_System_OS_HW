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

static void child_fn(struct timespec *buf, int nrecord, unsigned long nloop_per_resol, struct timespec start)
{
        int i;
        for (i = 0; i < nrecord; i++) {
                struct timespec ts;

                load(nloop_per_resol);
                clock_gettime(CLOCK_MONOTONIC, &ts);
                buf[i] = ts;
        }
        for (i = 0; i < nrecord; i++) {
                printf("%d\t%ld\n", pthread_self(), diff_nsec(start, buf[i]) / NSECS_PER_MSEC);
        }
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
void *child() {
  pthread_mutex_lock(&mutex);
  struct timespec start;

  int nrecord = 5;
  struct timespec *logbuf = malloc(nrecord * sizeof(struct timespec));
  unsigned long nloop_per_resol = loops_per_msec();

  clock_gettime(CLOCK_MONOTONIC, &start);
  child_fn(logbuf, nrecord, nloop_per_resol, start);

  pthread_mutex_unlock(&mutex);
}

// 主程式
int main() {
  pthread_t t[2]; // 宣告 pthread 變數

  for (int i = 0; i < 3; i++){
    memset(&t[i], 0, sizeof(t[i]));
    pthread_create(&t[i], NULL, child, NULL); // 建立子執行緒
  }

  for (int i = 0; i < 2; i++)
    pthread_join(t[i], NULL); // 等待子執行緒執行完成
  return 0;
}