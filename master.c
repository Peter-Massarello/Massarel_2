#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>


void help_menu() {
	printf("HELP MENU TO BE ADDED HERE\n\n");
}




int main(int argc, char* argv[]){

	int opt;
	if (argc == 1)
	{
		system("clear");
		printf("Program called with no arguments, use ./master -h for help\n");
		return 0;
	}
	while((opt = getopt(argc, argv, "h")) != -1)
	{
		system("clear");
		switch(opt)
		{
			case 'h':
				help_menu();
				break;
		}
	}
	return 0;
}
