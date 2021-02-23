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
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_KEY 1234

typedef enum {idle, want_in, in_cs} state;

extern int errno;
int new_count, shmid, shmid2, shmid3;
int *shmptr, *shmptr3;
state *shmptr2;
int max_children = 19; // Default
int max_time = 100; // Default
int shmid = 0;

void ctrl_c(){
	printf("Signal caught\n");
	exit(0);
}

void exit_func(){
	printf("Program exectuion time has run out\n");	
	exit(0);
}

void help_menu() {
	printf("HELP MENU:\n\n");
	printf("Program will take in a filename argument (datafile) and two optional arguments (-s, -t).\n");
	printf("The program will use fork() and exec() to create multiple processes that will add up the sum of a full binary tree\n\n");
	printf("PROGRAM OPTIONS:\n");
	printf("OPTION [-s]:\n");
	printf("           When called, will set the max number of child processes. Default is 20 processes in total.\n");
	printf("           Will create only up to 19 child processes, counting ./master in the total 20.\n");
	printf("           If more than 20 is given, count resets to default\n");
	printf("           EX: ./master -s 15 datafile\n\n");
	printf("OPTION [-t]:\n");
	printf("	   When called, will set the max life-time of the program. Default is 100 seconds.\n");
	printf("           There is no max so whatever value given to -s will override default;\n");
	printf("           EX: ./master -t 150 datafile\n\n");
}

void is_power_of_2(int num){
	bool flag = false;
	if (num == 0)
	{
		errno = 5;
		perror("master: Error: No numbers in file, no sum");
		exit(0);
	}
	else if (num == 1)
	{
		printf("Only one number in datafile\nSum is that number\n\n");
		exit(0);
	}
	else if (ceil(log2(num)) == floor(log2(num)))
		new_count = num;
	else
	{
		while(!flag)
		{
			num++;
			if (ceil(log2(num)) == floor(log2(num)))
			{
				new_count = num;
				flag = true;
			}	
		}
	
	}
	
}

int get_depth(int nodes){
	int temp;
	temp = log2(nodes);

	return temp;
}

int get_num_count(char *file_name){
	FILE *fp;
	char ch;
	int count = 0;

	fp = fopen(file_name, "r");
	if (fp == NULL) 
	{
		errno = 2;
		perror("master: Error: File name not found ");
		exit(0);
	}
	else
	{
		while((ch = fgetc(fp)) != EOF)
		{
			if (ch == '\n')
				count++;
		}
	}
	fclose(fp);
	return count;
}

void max_check(int num){
	if (num > 19)
		max_children = 19; // Resets back to default if too many children are given
}

int main(int argc, char* argv[]){
	int count, opt, slot_num, index, shmindex, depth;	
	char buf[100];
	char ch;
	char *opt_buf;
	FILE *fp;

	signal(SIGINT, ctrl_c);
	signal(SIGALRM, exit_func);

	// If no arguments given end program, otherwise move on
	if (argc == 1)
	{
		system("clear");
		printf("Program called with no arguments, use ./master -h for help\n");
		return 0;
	}
	
//**************************************************************************************
//
//	Uses getopt to loop through arguments given and perform the appropriate option
//
//**************************************************************************************

	while((opt = getopt(argc, argv, "hs:t:")) != -1)
	{
		system("clear");
		switch(opt)
		{
			case 'h':
				help_menu();
				break;
			case 's':
				opt_buf = optarg;
				max_children = atoi(opt_buf);
				max_check(max_children);
				printf("Children set...\n\n");
				break;
			case 't':
				opt_buf = optarg;
				max_time = atoi(opt_buf);
				alarm(max_time);
				printf("%d\n", max_time);
				printf("Time set...\n\n");
				break;
		}
	}
	
//****************************************************************************************
//
//	Performs the check on the count of numbers from the data file
//	If number is not a power of 2, adds zeros to make up for it
//
//****************************************************************************************
	// Get total number count from number file
	count = get_num_count(argv[argc-1]);

	is_power_of_2(count);
	printf("%d\n", new_count);

	depth = get_depth(new_count);

//****************************************************************************************
//
//	Creates valid shared memory
//
//****************************************************************************************
	key_t key = ftok("./README.md", 'a');	
	shmid = shmget(key, sizeof(int) * new_count, IPC_CREAT | 0666);
	
	if (shmid < 0)
	{
		errno = 5;
		perror("master: Error: Could not create shared memory");
		exit(0);
	}
	
	shmptr = (int *)shmat(shmid, 0, 0);
	
	if (shmptr == (int *) -1)
	{
		errno = 5;
		perror("master: Error: Could not attach shared memory");
		exit(0);
	}

//***************************************************************************************
//
//	Writes numbers from data file into shared memory
//
//***************************************************************************************

	fp = fopen("datafile", "r");
	if (fp == NULL)
	{
		errno = 2;
		perror("master: Error: File could not be found");
		exit(0);
	}
	// Set both indexes to 0
	index = 0; 
	shmindex = 0;
	while((ch = fgetc(fp)) != EOF)
	{
		if (ch != '\n')
		{
			buf[index] = ch;
			index++;
		}
		else
		{
			buf[index] = '\0';
			shmptr[shmindex] = atoi(buf);
			//printf("%d\n", shmptr[shmindex]);
			shmindex++;
			index = 0;	
		}
	}
	fclose(fp);
	for (int i = shmindex; i < new_count; i++)
	{
		shmptr[i] = 0;
		//printf("%d\n", shmptr[i]);
	}
	

//************************************************************************************
//
//	Goes through each depth of the array and calls the necessary indexes
//
//************************************************************************************
	
	pid_t wpid;
	int status = 0;

	/*if (fork() == 0)
	{
		
	}	
	else
	{
		while((wpid = wait(&status)) > 0);
		printf("All children done and back into master\n");
	}*/

	char size[100];
	snprintf(size, sizeof(size), "%d", new_count);
	int power = 1;
	int jump;
	for (int i = depth; i > 0; i--)
	{
		printf("Depth is %d\n", i);
		jump = pow(2, power); 
		for (int j = 0; j < new_count; j = jump + j)
		{
			if (fork() == 0)
			{
				
				char index[100];
				char depth[100];// char buffer to turn ints into char arrays
				
				snprintf(index, sizeof(index), "%d", j);
				snprintf(depth, sizeof(depth), "%d", i);
				execlp("./bin_adder", index, depth, size, (char *)0);
			}
			else
			{
				while((wpid = wait(&status)) > 0);
				printf("%d\n", shmptr[j]);

			}
		}
		printf("\n\n");
		power++;
	}

	printf("Past exec\n");	

	return 0;
}
