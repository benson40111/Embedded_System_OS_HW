#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<fcntl.h>
#include<sys/mman.h>

int main()
{
    int size = 0, fd = 0;
	char buffer[100];
    struct timeval tv1, tv2;
	char s[] = "This is hello";
    
    //read

    gettimeofday(&tv1, NULL);
    fd = open("testfile", O_WRONLY);

	write(fd, s, sizeof(s));

    close(fd);
	fd = open("testfile", O_RDONLY);
	size = read(fd, buffer, sizeof(buffer));
	close(fd);
    gettimeofday(&tv2, NULL);

    printf("Time of read/write: %dms\n", tv2.tv_usec - tv1.tv_usec);
	printf("Write text is:%s\n", buffer);
    
    //mmap

    gettimeofday(&tv1, NULL);
    fd = open("testfile", O_RDWR);
	size = read(fd, buffer, sizeof(buffer));
	char *map = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	munmap(map, size);
	close(fd);
    gettimeofday(&tv2, NULL);
	printf("Read text is:%s\n", buffer);
    printf("Time of mmap: %dms\n", tv2.tv_usec - tv1.tv_usec);
    return 0;
}
