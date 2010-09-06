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

int listen_for_inbound_requests(int port, FILE *filename_fd)
{
	int listenfd, bytes_sent;
	int connfd;
	char buff[1024];
	socklen_t len;
	struct sockaddr_in serv, cli;	
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
	printf("Listening on port %d...\n", ntohs(serv.sin_port));

	if ((bind(listenfd, (struct sockaddr *) &serv, sizeof(serv))) == -1){
		perror("Bind error");
	}

	if ((listen(listenfd, 50)) == -1){
		perror("Listen error");
	}

	for (;;) {
		len = sizeof(cli);
		connfd = accept(listenfd, (struct sockaddr *) &cli, &len);
		printf("Connection from %s, port %d\n", inet_ntop(AF_INET, &cli.sin_addr, buff, sizeof(buff)),ntohs(cli.sin_port));

		if (!(bytes_sent = send_file(filename_fd, connfd)))
			printf("Error transferring file.\n");
		else
			printf("Finished transfer to %s. %d bytes sent.\n", inet_ntop(AF_INET, &cli.sin_addr, buff, sizeof(buff)), bytes_sent);
	}

	return 0;
}

/*
 * Send the file to the currently connected client, return number of bytes sent or 0 on error.
 */

int send_file(FILE *fd, int connfd)
{
	int filesize,
	    number_complete_chunks,
	    last_chunk_size,
	    total_bytes_sent = 0,
	    bytes_sent = 0,
	    i;

	fseek(fd, 0L, SEEK_END);
	filesize = ftell(fd);
	rewind(fd);

	number_complete_chunks = filesize / CHUNK_SIZE;
	last_chunk_size = filesize % CHUNK_SIZE;

	char chunk[CHUNK_SIZE];

	for (i = 0; i < number_complete_chunks; i++)
	{
		fread(chunk, CHUNK_SIZE, 1, fd);

		if ((bytes_sent = send(connfd, chunk, sizeof(chunk), 0)) < 0)
			return -1;

		total_bytes_sent += bytes_sent;
		if(be_verbose)
			printf("%d / %d\n", total_bytes_sent, filesize);
	}

	// send the last incomplete chunk at the end of the file.
	fread(chunk, last_chunk_size, 1, fd);

	if ((bytes_sent = send(connfd, chunk, last_chunk_size, 0)) < 0)
		return -1;

	total_bytes_sent += bytes_sent;
	if(be_verbose)
		printf("%d / %d\n", total_bytes_sent, filesize);

	return total_bytes_sent;
}

