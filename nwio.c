#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "LiDE.h"

/*
 * Send the file to the currently connected client, return number of bytes sent or 0 on error. At the minute
 * if there is an error with a send call it just dies and returns -1, this could obviously be improved to 
 * make it a little "smarter".
 */

int send_file(FILE *fd, int connfd)
{
	int filesize,
	    number_complete_chunks,
	    final_chunk_size,
	    total_bytes_sent = 0,
	    bytes_sent = 0,
	    i;

	fseek(fd, 0L, SEEK_END);
	filesize = ftell(fd);
	rewind(fd);

	number_complete_chunks = filesize / CHUNK_SIZE;
	final_chunk_size = filesize % CHUNK_SIZE;

	char *chunk;

	if((chunk = (char *) malloc(CHUNK_SIZE)) == NULL)
	{
		perror("Malloc");
		exit(1);
	}


	for (i = 0; i < number_complete_chunks; i++)
	{
		fread(chunk, CHUNK_SIZE, 1, fd);

		if ((bytes_sent = send(connfd, chunk, CHUNK_SIZE, 0)) < 0)
			return -1;

		total_bytes_sent += bytes_sent;
		if(be_verbose)
			printf("%d / %d\r", total_bytes_sent, filesize); // the "\r" refreshes current line
	}

	// send the last incomplete chunk at the end of the file.
	fread(chunk, final_chunk_size, 1, fd);

	if ((bytes_sent = send(connfd, chunk, final_chunk_size, 0)) < 0)
		return -1;

	total_bytes_sent += bytes_sent;
	if(be_verbose)
		printf("%d / %d\n", total_bytes_sent, filesize);

	free(chunk);
	return total_bytes_sent;
}

/*
 * We wait for someone to ask for our file.
 */

int listen_for_inbound_requests(int port, FILE *filename_fd)
{
	int listenfd, bytes_sent;
	int connfd;
	struct sockaddr_in serv, cli;	
	socklen_t len;

	char *ip_buff; // Somewhere handy to put IP addresses.
	
	if((ip_buff = (char *) malloc(INET_ADDRSTRLEN)) == NULL)
	{
		perror("Malloc");
		exit(1);
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Error creating socket");
	}

	int tr = 1;
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) { 
		perror("setsockopt");
		exit(1);
	}

	bzero(&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	serv.sin_port = port;

	if ((bind(listenfd, (struct sockaddr *) &serv, sizeof(serv))) == -1){
		perror("Bind error");
		exit(1);
	} 
	

	if ((listen(listenfd, 5)) == -1){ // We don't need a huge backlog (arg 2). 
		perror("Listen error");
		exit(1);
	}

	printf("Listening on port %d...\n", ntohs(serv.sin_port));
	

	for (;;) {
		len = sizeof(cli);
		connfd = accept(listenfd, (struct sockaddr *) &cli, &len);

		inet_ntop(AF_INET, &(cli.sin_addr), ip_buff, INET_ADDRSTRLEN);
		printf("Connection from %s, port %d\n", ip_buff, ntohs(cli.sin_port));

		if ((bytes_sent = send_file(filename_fd, connfd)) < 0)
			printf("Error transferring file.\n");
		else
			printf("Finished transfer to %s. %d bytes sent.\n", ip_buff, bytes_sent);exit(0);
	}

	free(ip_buff);

	return 0;
}
