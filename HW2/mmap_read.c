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
    int std_size = 0, std_fd = 0;
	char buffer[100];
    struct timeval tv1, tv2;
	char s[] = "This is hello";
    
	printf("Standard I/O...\n");
    //read

    gettimeofday(&tv1, NULL);
    std_fd = open("testfile_write", O_WRONLY);
	write(std_fd, s, sizeof(s));
    close(std_fd);
	std_fd = open("testfile_write", O_RDONLY);
	std_size = read(std_fd, buffer, sizeof(buffer));
	close(std_fd);
    gettimeofday(&tv2, NULL);

	printf("Read text is:%s\n", buffer);
    printf("Time of read/write: %dms\n", tv2.tv_usec - tv1.tv_usec);
    
    //mmap
	printf("mmap...\n");

	char buffer1[100];
	int fd = 0;
	size_t size = sizeof(char);
    gettimeofday(&tv1, NULL);
    fd = open("testfile_mmap", O_RDWR|O_TRUNC);
	size_t textsize = strlen(s) + 1;
	if (lseek(fd, size, SEEK_SET) == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }
	if (write(fd, "", 1) == -1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
	char *map;
	map = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	for (size_t i = 0; i < textsize; i++)
        map[i] = s[i];
    

	if (msync(map, size, MS_SYNC) == -1)
    {
        perror("Could not sync the file to disk");
    }
    
	close(fd);
    gettimeofday(&tv2, NULL);

	printf("Read text is:%s\n", map);
    printf("Time of mmap: %dms\n", tv2.tv_usec - tv1.tv_usec);
	if (munmap(map, size) == -1)
    {
        close(fd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    return 0;
}
