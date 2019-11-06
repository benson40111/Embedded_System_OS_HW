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
int cpu_time = 0;

static void child_fn(int id, int total)
{
    for (int i = 0; i < total; i++) {
        cpu_time++;
	    printf("Process %d\t%d\n", getpid(), cpu_time);
    }
    exit(EXIT_SUCCESS);
}

static void parent_fn(int nproc)
{
    int i;
    for (i = 0; i < nproc; i++)
	wait(NULL);
}

static pid_t *pids;

int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;

    if (argc < 3) {
	fprintf(stderr, "usage: %s <nproc> <total[ms]>\n",
		argv[0]);
	exit(EXIT_FAILURE);
    }

    int nproc = atoi(argv[1]);
    int total = atoi(argv[2]);

    if (nproc < 1) {
	fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
	exit(EXIT_FAILURE);
    }

    if (total < 1) {
	fprintf(stderr, "<total>(%d) should be >= 1\n", total);
	exit(EXIT_FAILURE);
    }
    
    fflush(stdout);

    pids = malloc(nproc * sizeof(pid_t));
    if (pids == NULL) {
	    warn("malloc(pids) failed");
    }

    int ncreated;
    for (int i = 0, ncreated = 0; i < nproc; i++, ncreated++) {
        pids[i] = fork();
        if (pids[i] > 0)
            printf("Process %d(%d) was created...\n", pids[i], i);
        
        pthread_mutex_lock(&mutex);
        if (pids[i] < 0) {
            goto wait_children;
        } else if (pids[i] == 0) {
            // children

            child_fn(i, total);
            /* shouldn't reach here */
        }
        pthread_mutex_unlock(&mutex);
    }
    ret = EXIT_SUCCESS;

    // parent

  wait_children:
    if (ret == EXIT_FAILURE)
	for (int i = 0; i < ncreated; i++)
	    if (kill(pids[i], SIGINT) < 0)
		warn("kill(%d) failed", pids[i]);

    for (int i = 0; i < ncreated; i++)
	if (wait(NULL) < 0)
	    warn("wait() failed.");

  free_pids:
    free(pids);

    exit(ret);
}
