#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "hermes.h"

void print_help(void){
	printf("Hermes - v0.1\n\nhermes [-h] [-p <port>] -f <filename>\n \
			-h: Prints this help.\n \
			-v: Be verbose.\n \
			-p: Specify a port to listen on. Default 9999.\n");
	exit(0);
}

int main(int argc, char *argv[]){

	be_verbose = 0;

	FILE *filename_fd;

	char *filename_to_serve;
	char *logfile;

	int listen_port = htons(9999);
	if (argc < 2)
		print_help();

	char c;
	while ((c = getopt(argc, argv, "hp:v")) != -1) {

		switch(c){

			case 'h':
				// Help.
				print_help();
				break;
			case 'v':
				printf("Being verbose.\n");
				be_verbose = 1;
				break;
			case 'p':
				if ((optarg != NULL) && (optarg[0] != '\0') ){

					listen_port = htons(atoi(optarg));
					printf("listen_port: %d\n", listen_port);

					if (listen_port < 1024 && getuid() > 0){
						printf("Binding to a port lower than 1024 requires superuser priveleges. You, unfortunately, are not so priveleged. Sorry!\n");
						exit(0);
					}
					else {
						listen_port = htons(atoi(optarg));
						break;
					}
				}
				else break;

			default:
				print_help();
				break;
		}
	}

	// Get the filename, which is the argument without an option.
	if ((argc - optind) == 1)
	{
		if (strlen(argv[argc-1]) > 255)
		{
			printf("Filename too long.\n");
			exit(0);
		}
		filename_to_serve = malloc(strlen(argv[argc-1]) + 1);

		if (filename_to_serve != NULL){
			strcpy(filename_to_serve, argv[argc-1]);
		}
	}


	if ((filename_fd = fopen(filename_to_serve, "r")) != NULL){
		int pos = 0;
		listen_for_inbound_requests(listen_port, filename_fd);
		printf("%d\n", listen_port);
	}

	else
		perror("Error opening file");

	return 0;
}


