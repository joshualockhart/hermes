#define CHUNK_SIZE 1024
char buffer[CHUNK_SIZE];
int be_verbose;

void print_help(void);
char *read_file(FILE *filename_fd, int from);
