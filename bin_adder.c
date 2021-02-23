#define _XOPEN_SOURCE 700
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 1234

extern int errno;

typedef enum {idle, want_in, in_cs} state;

int arrid, stateid, turnid, index, power, id;
int *arrptr;
state *stateptr;
int *turnptr;

void kill_func(){
	shmdt(arrptr);
	shmdt(stateptr);
	shmdt(turnptr);
	exit(0);
}


void crit_sec(){
	FILE *fp;
	time_t rawtime;
	struct tm * timeinfo;

	fp = fopen("./adder_log", "a");
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("IN critical section \n");
	int next_index = index + pow(2, power);

	fprintf(fp, "Index's being added are %d and %d at the time of %s\n", arrptr[index], arrptr[next_index], asctime(timeinfo));
	fclose(fp);
	arrptr[index] = arrptr[index] + arrptr[next_index];
}

int main(int argc, char *argv[]){
	index = atoi(argv[0]);
	int depth = atoi(argv[1]);
	int arr_size = atoi(argv[2]);
	id = atoi (argv[3]);
	int max_children = atoi(argv[4]);
	power = atoi(argv[5]);
	power--;

	key_t key = ftok("./README.md", 'a');
	arrid = shmget(key, sizeof(int) * arr_size, IPC_EXCL);

	if (arrid < 0)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be created");
		exit(0);
	}

	arrptr = (int *)shmat(arrid, 0 ,0);

	if (arrptr == (int *) -1)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be attached");
		exit(0);
	}
	
	key_t key2 = ftok("./Makefile", 'a');
	stateid = shmget(key2, sizeof(state) * max_children, IPC_EXCL);

	if (stateid < 0)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be created");
		exit(0);
	}

	stateptr = (state *)shmat(stateid, 0, 0);

	if (stateptr == (state *) -1)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be attached");
		exit(0);
	}

	key_t key3 = ftok("./datafile", 'a');
	turnid = shmget(key3, sizeof(int), IPC_EXCL);

	if (turnid < 0)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be created");
		exit(0);
	}

	turnptr = (int *)shmat(turnid, 0, 0);

	if (turnptr == (int *) -1)
	{
		errno = 5;
		perror("bin_adder: Error: Shared memory could not be created");
		exit(0);
	}

	//printf("Sum of both is %d + %d\n", shmptr[index], shmptr[index + 1]);

	int j;
	do{
		do{
		printf(" ");
			stateptr[id] = want_in;
			j = *turnptr;
			while (j != id)
			{
				j = (stateptr[j] != idle) ? *turnptr : (j + 1) % max_children;
			}
			stateptr[id] = in_cs;
			for (j = 0; j < max_children; j++)
			{
				if ((j != id) && (stateptr[j] == in_cs))
				{
					break;
				}
			}
		} while ((j < max_children) || (*turnptr != id && stateptr[*turnptr] != idle));
		*turnptr = id;
		crit_sec();

		j = (turnptr[0] + 1) % max_children;
		while (stateptr[j] == idle)
		{
			j = (j + 1) % max_children;
		}
		*turnptr = j;
		stateptr[id] = idle;
		kill_func();
	} while(1);

	kill_func();

	return 0;
}
