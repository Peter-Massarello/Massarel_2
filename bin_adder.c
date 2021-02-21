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




int main(int argc, char *argv[]){

	printf("Hello from bin_adder\n");
	return 0;
}
