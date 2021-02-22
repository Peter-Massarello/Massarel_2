#define _XOPEN_SOURCE 700
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 1234

extern int errno;


int main(int argc, char *argv[]){
	int index = atoi(argv[1]);
	int depth = atoi(argv[2]);
	int arr_size = atoi(argv[2]);

	int shmid;
	int *shmptr;

	key_t key = ftok("./README.md", 'a');
	shmid = shmget(key, sizeof(int) * arr_size, IPC_EXCL | 0666);
	if (shmid == -1)
	{
		errno = 3;
		perror("master: Error: Shared memory could not be created");
		exit(0);
	}
	shmptr = (int *)shmat(shmid, 0 ,0);
	if (shmptr == (int *)-1)
	{
		errno = 3;
		perror("master: Error: Shared memory could not be attached");
		exit(0);
	}
	


	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
