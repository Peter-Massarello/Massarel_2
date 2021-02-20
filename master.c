#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

extern int errno;
int new_count;

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
	}
	else
	{
		while((ch = fgetc(fp)) != EOF)
		{
			if (ch == '\n')
				count++;
		}
	}
	return count;
}

int main(int argc, char* argv[]){
	int max_children = 20; // Default
	int max_time = 100; // Default
	int count, opt, slot_num;

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

	while((opt = getopt(argc, argv, "hs:i:")) != -1)
	{
		system("clear");
		switch(opt)
		{
			case 'h':
				help_menu();
				break;
			case 's':
				printf("-s function placeholder\n\n");
				break;
			case 'i':
				printf("-i function placeholder\n\n");
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

	return 0;
}
