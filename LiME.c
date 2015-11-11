#include <stdio.h>        // C library to perform Input/Output operations
#include <unistd.h>       //
#include <string.h>       // C Strings
#include <stdlib.h>       // C Standard General Utilities Library
#include <arpa/inet.h>    //
#include <sys/socket.h>   //
#include <sys/types.h>    //
#include "LiDE.h"         //

void print_help(void){
	printf("\x1B[33mLiDE (\033[1mLi\033[22mnux \033[1mD\033[22misk \033[1mE\033[22mxtractor) - v0.0.1.5\033[0m\n\n\
LiDE is a very small fileserver.\n\
To host a file, do\n\n\
$ \x1B[32m./lide <filename>\033[0m              Will use default port 31337\n\
    or\n\
$ \x1B[32m./lide -p <port> <filename>\033[0m    To specify a port to send on.\n\n\
$ \x1B[32m./lide [-h]\033[0m                    Prints this help.\n\n\
Then to retrieve it on another host, do something like\n\n\
$ \x1B[32mnc <LiDE Host IP> 31337 > retrieved_filename\033[0m\n");
	exit(0);
}

int main(int argc, char *argv[])
{

	be_verbose = 1;
	FILE *filename_fd;
	char *filename_to_serve;
	int listen_port = htons(31337);

	if (argc < 2)
		print_help();

	char c;
	while ((c = getopt(argc, argv, "hp:")) != -1) {
		switch(c){

			case 'h':
				print_help();
				break;
			case 'p':
				if ((optarg != NULL) && (optarg[0] != '\0')){

					listen_port = htons(atoi(optarg));

					if (listen_port < 1024 && getuid() > 0){
						printf("Invalid port number. Binding to a port lower than 1024 requires root priveleges.\n");
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

		if((filename_to_serve = malloc(strlen(argv[argc-1]) + 1)) == NULL)
		{
			perror("Malloc");
			exit(1);
		}
		strncpy(filename_to_serve, argv[argc-1], strlen(argv[argc-1]));
	}


	if ((filename_fd = fopen(filename_to_serve, "r")) != NULL){
		listen_for_inbound_requests(listen_port, filename_fd);
		printf("%d\n", listen_port);
	}

	else
		perror("Error opening file");

	free(filename_to_serve);


	return 0;
}

