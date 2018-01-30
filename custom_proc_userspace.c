#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define dev "/proc/mytest"
//   /* dbg */ #define dev "text.txt"

int main(void)
{
	int fd;
	char buffer[32];
	int count = 12; 
	int rv;
	char message[] = "Hello";
	
	fd = open(dev, O_RDWR, S_IWUSR | S_IRUSR);

	printf("MAIN PROCESS with pid: %d\n",getpid());

	if (fd == -1) {
		printf("Error on opening file\n");
		exit (-1);
	}

	printf("%s opened\n",dev);

	// Performing a read,write,read in order.
/*	rv = read(fd, buffer, count);		

	if (rv == -1) { 
		printf("Error while reading\n");
		exit(-1);
	}
	
	printf("Readed:%d bytes:(%s)\n", rv, buffer);
///	
	strcpy(buffer,"hello");
	printf("Will try to write: %s\n",buffer);	

	rv = write(fd, buffer, 5);	

	if (rv == -1) {
		printf("Error on write\n");
		printf("%s\n",errno);
		exit(-1);
	}

	printf("written :%d bytes:(%s)\n", rv, message);

	memset(buffer, 0, 32);
*/
	rv = read(fd, buffer, count);		

	if (rv == -1) { 
		printf("Error while reading\n");
		exit(-1);
	}
	
	printf("Readed:%d bytes:(%s)\n", rv, buffer);

	rv = close(fd);

	if (rv == -1) {
		printf("Error on closing file\n");
		exit (-1);
	}
	
	printf("%s closed\n",dev);

	return (0);
}
