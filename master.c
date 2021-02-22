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
int new_count;
int max_children = 20; // Default
int max_time = 100; // Default

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
	printf("PROGRAM OPTIONS:\n");
	printf("OPTION [-");
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

int main(int argc, char* argv[]){
	int count, opt, slot_num, shmid, index, shmindex;	
	char buf[100];
	char ch;
	char *time_buf;
	int *shmptr;
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
				max_children = *optarg;
				printf("Children set...\n\n");
				break;
			case 't':
				time_buf = optarg;
				max_time = atoi(time_buf);
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
	sleep(300);
	// Get total number count from number file
	count = get_num_count(argv[argc-1]);

	is_power_of_2(count);
	printf("%d\n", new_count);

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
			printf("%d\n", shmptr[shmindex]);
			shmindex++;
			index = 0;	
		}
	}
	fclose(fp);
	for (int i = shmindex; i < new_count; i++)
	{
		shmptr[i] = 0;
		printf("%d\n", shmptr[i]);
	}
	char pass_buf[50];
	sprintf(pass_buf, "%d", new_count);
	printf("%s\n", pass_buf);
	execl("./bin_adder", "0", "0", pass_buf, NULL);
	printf("Past exec\n");	

	return 0;
}
