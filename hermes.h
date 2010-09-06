#define CHUNK_SIZE 1024
char buffer[CHUNK_SIZE];
int be_verbose;

int listen_for_inbound_requests(int port, FILE *filename_fd);
int send_chunk(int connfd, char *buffer);
int send_file(FILE *fd, int connfd);
void print_help(void);
char *read_file(FILE *filename_fd, int from);
int logger(FILE *logfile_fd, char *tolog);
