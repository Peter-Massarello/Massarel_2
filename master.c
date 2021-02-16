#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>

extern int errno;

void help_menu() {
	printf("HELP MENU TO BE ADDED HERE\n\n");
}


void print_file(char *file_name){
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
			if (ch == ' ' || ch == '\n')
				count++;
			else
			{
				
			}
		}
	}
	printf("%d\n", count);
}

int main(int argc, char* argv[]){

	int opt;
	if (argc == 1)
	{
		system("clear");
		printf("Program called with no arguments, use ./master -h for help\n");
		return 0;
	}
	while((opt = getopt(argc, argv, "hp")) != -1)
	{
		system("clear");
		switch(opt)
		{
			case 'h':
				help_menu();
				break;
			case 'p':
				print_file(argv[argc - 1]);
				break;
		}
	}
	return 0;
}
