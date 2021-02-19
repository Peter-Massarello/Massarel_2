#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>

extern int errno;

void help_menu() {
	printf("HELP MENU:\n\n");
	printf("PROGRAM OPTIONS:\n");
	printf("OPTION [-");
}

int power_of_2_check(int num){
	if (num == 0)
		return 0;
	else if (num == 1)
		return 1;
	else if (num % 2 == 0)
		return 2;
	else if (num % 2 != 0)
		return 3;
	
}

int get_num_count(char *file_name){
	FILE *fp;
	char ch;
	int count = 0;

	fp = fopen(file_name, "r");
	if (fp == NULL) 
	{
		errno = 2;
		perror("Error: ");
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
	// Get total number count from number file
	count = get_num_count(argv[argc-1]);

//****************************************************************************************
//
//	Performs the check on the count of numbers from the data file
//	If number is not a power of 2, adds zeros to make up for it
//
//****************************************************************************************

	if (power_of_2_check(count) == 0)
	{
		errno = 22;
		perror("Error: There were no numbers in the file");
		return 0;
	}
	else if (power_of_2_check(count) == 1)
	{
		errno = 22;
		perror("Error: Only one number in file, sum is that number");
		return 0;
	}
	else if (power_of_2_check(count) == 2)
	{
		slot_num = count;
		printf("%s\n", slot_num);
	}
	else if (power_of_2_check(count) == 3)
	{
		printf("Number is not a power of 2\n");
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
	return 0;
}
