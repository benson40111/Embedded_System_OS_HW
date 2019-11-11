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
    
	printf("Standard I/O...\n");
    //read

    gettimeofday(&tv1, NULL);
    fd = open("testfile_write", O_WRONLY);
	write(fd, s, sizeof(s));
    close(fd);
	fd = open("testfile_write", O_RDONLY);
	size = read(fd, buffer, sizeof(buffer));
	close(fd);
    gettimeofday(&tv2, NULL);

	printf("Read text is:%s\n", buffer);
    printf("Time of read/write: %dms\n", tv2.tv_usec - tv1.tv_usec);
    
    //mmap
	printf("mmap...\n");

	char buffer1[100];
	size = 10;
    gettimeofday(&tv1, NULL);
    fd = open("testfile_mmap", O_RDWR);
	char *map = (char*)malloc(size * sizeof(char));
	map = (char*)mmap(NULL, sizeof(char)*size, PROT_WRITE, MAP_SHARED, fd, 0);
	map = s;
	munmap(map, sizeof(char)*size);
	close(fd);
    gettimeofday(&tv2, NULL);

	printf("Read text is:%s\n", map);
    printf("Time of mmap: %dms\n", tv2.tv_usec - tv1.tv_usec);
	memset(map, 0, size * sizeof(char));
    return 0;
}
