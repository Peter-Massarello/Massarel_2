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

//*****************************************************************************
//
//	Assingment 2, Peter Massarello CMPSCI 4760
//	Due Date - 2/23/21
//
//*****************************************************************************

typedef enum {idle, want_in, in_cs} state;

extern int errno;
pid_t *pid_list;
int new_count, shmid, shmid2, shmid3;
int pid_count = 0;
int *shmptr, *shmptr3;
state *shmstate; // Holds state for each process, taken from Sol. 4
int max_children = 19; // Default
int max_time = 100; // Default

void kill_pids(){
	for (int i = 0; i < max_children; i++)
	{
		if(pid_list[i] != 0)
			kill(pid_list[i], SIGKILL);
	}

}

void kill_func(){
	shmdt(shmptr);
	shmctl(shmid, IPC_RMID, NULL);
	shmdt(shmstate);
	shmctl(shmid2, IPC_RMID, NULL);
	shmdt(shmptr3);
	shmctl(shmid3, IPC_RMID, NULL);
	kill_pids();
	free(pid_list);
}


void ctrl_c(){
	kill_func();
	exit(0);
}

void timer_func(){
	kill_func();
	exit(0);
}

void init_pid(){ // Sets all pid's to 0
	for (int i = 0; i < max_children; i++)
		pid_list[i] = 0;
}

void init_state(){ // Sets every process to idle
	for (int i = 0; i < max_children; i++)
		shmstate[i] = idle;
}

void help_menu() {
	printf("HELP MENU:\n\n");
	printf("Program will take in a filename argument (datafile) and two optional arguments (-s, -t).\n");
	printf("Program will create a new datafile that it will pull numbers out of.\n");
	printf("If datafile already exists, new runs will append new entries to the file\n");
	printf("Reccomend using 'make clean' before every run of the program\n\n");
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
	exit(0);
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

int find_empty(){
	for (int i = 0; i < max_children; i++)
	{
		if (pid_list[i] == 0)
			return i;
	}
}

bool process_check(){
	bool flag = true;
	
	for (int i = 0; i < max_children; i++)
	{
		if (pid_list[i] != 0)
			flag = false;
	}

	return flag;
}

void remove_pid(pid_t pid){
	for (int i = 0; i < max_children; i++)
	{
		if (pid_list[i] == pid)
			pid_list[i] = 0;
	}
}

void my_sigchild_handler(int sig){
	pid_t p;
	int status;

	while((p = waitpid(-1, &status, WNOHANG)) != -1)
	{
		pid_count--;
		remove_pid(p);
	}
}

int main(int argc, char* argv[]){

//******************************************************************************************
//
//	Variable creation
//
//******************************************************************************************
	int count, opt, slot_num, index, shmindex, depth;	
	char buf[100];
	char ch;
	char *opt_buf;
	FILE *fp;
	

//******************************************************************************************
//
//	Signal Creation
//	
//******************************************************************************************

	signal(SIGINT, ctrl_c);
	signal(SIGALRM, timer_func);
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = my_sigchild_handler;

	sigaction(SIGCHLD, &sa, NULL);

//******************************************************************************************
//
//	Loop that creates data file
//
//******************************************************************************************

	// If no arguments given end program, otherwise move on
	if (argc == 1)
	{
		system("clear");
		printf("Program called with no arguments, use ./master -h for help\n");
		return 0;
	}

	for (int i = 0; i < 32; i++)
	{
		system("echo $((RANDOM % 256)) >> datafile");
	}
	
//**************************************************************************************
//
//	Uses getopt to loop through arguments given and perform the appropriate option
//
//**************************************************************************************
	system("clear");
	while((opt = getopt(argc, argv, "hs:t:")) != -1)
	{
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

	depth = get_depth(new_count);

	pid_list = malloc(sizeof(pid_t) * max_children);
	init_pid();

//****************************************************************************************
//
//	Creates valid shared memory for array, state, and flag
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

	key_t key2 = ftok("./Makefile", 'a');
	shmid2 = shmget(key2, sizeof(state) * max_children, IPC_CREAT | 0666);
	
	if (shmid2 < 0)
	{
		errno = 5;
		perror("master: Error: Could not create shared memory");
		exit(0);
	}
	
	shmstate = (state *)shmat(shmid2, 0, 0);
	
	if (shmstate == (state *) -1)
	{
		errno = 5;
		perror("master: Error: Could not attach shared memory");
		exit(0);
	}

	init_state();

	key_t key3 = ftok("./datafile", 'a');
	shmid3 = shmget(key3, sizeof(int), IPC_CREAT | 0666);

	if (shmid3 < 0)
	{
		errno = 5;
		perror("master: Error: Could not create shared memory");
		exit(0);
	}

	shmptr3 = (int *)shmat(shmid3, 0, 0);

	if (shmptr3 == (int *) -1)
	{
		errno = 5;
		perror("master: Error: Could not create shared memory");
		exit(0);
	}

	shmptr[0] = -1;

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
	}
	

//************************************************************************************
//
//	Goes through each depth of the array and calls the necessary indexes
//
//************************************************************************************

	char sizestr[50];
	char depthstr[50];
	char indexstr[50];
	char idstr[50];
	char pwrstr[50];
	char chldstr[50];
	snprintf(sizestr, sizeof(sizestr), "%d", new_count); // Creates string param of the size of nodes
	int power = 1;
	int jump;
	int list_index;
	bool depth_complete = false;

	for (int i = depth; i > 0; i--)
	{
		while (pid_count != 0)
		{
			if (process_check()) // Resets count back to zero if count gets desynched from pid_list
				pid_count = 0;
		}
		depth_complete = false;
		printf("\nCurrent Depth is %d...\n", i);
		snprintf(depthstr, sizeof(depthstr), "%d", i); // Creates string param of the current depth
		jump = pow(2, power);
		do {
			if (pid_count == 0) // Initial check to start creation of process
			{
				for (int j = 0; j < new_count; j = jump + j)
				{
					if (pid_count < max_children)
					{
						pid_count++;
						snprintf(indexstr, sizeof(indexstr), "%d", j);
						list_index = find_empty();
						pid_list[list_index] = fork();
						if (pid_list[list_index] == 0)
						{
							snprintf(chldstr, sizeof(chldstr), "%d", max_children);
							snprintf(pwrstr, sizeof(pwrstr), "%d", power);
							snprintf(idstr, sizeof(idstr), "%d", list_index);
							execlp("./bin_adder", indexstr, depthstr, sizestr, idstr, chldstr, pwrstr, (char *)0);
						}
					}
				}
				printf("\nDepth of %d complete...\n", i);
				depth_complete = true;
			}
		} while(!depth_complete);
		power++;
	}
	
	printf("%d <- This is the final sum\n", (shmptr[0] + shmptr[new_count/2]));
	kill_func();
	return 0;
}
