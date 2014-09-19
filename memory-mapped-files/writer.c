
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

int main ()
{
	int fd;
	long page_size;
	char * mem;

	// Get the page size
	page_size = getpagesize();

	// Open file in read-write mode. Create if non existing.
	fd = open("mem_mapped_file.txt", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1)
	{
		perror("open");
		exit(1);
	}

	// Truncate file to the page size
	if (ftruncate(fd, page_size) == -1)
	{
		perror("ftruncate");
		exit(1);
	}

	// Map file to memory
	mem = mmap((caddr_t) 0, page_size,  PROT_WRITE, 
		MAP_SHARED, fd, 0);

	if (mem == (caddr_t) -1)
	{
		perror("mmap");
		exit(1);
	}

	// Copy some date to the file
	char * buffer = malloc(page_size);
	time_t timestamp;
	time(&timestamp);
	sprintf(buffer, "This is just some studpid text.\nTimestamp: %ld",
		timestamp);
	strcpy(mem, buffer);

	// Example of random access. Every second character, if alphabetic,
	// is transformed to its uppercase letter.
	int i;
	for (i = 0; i < page_size; i++)
	{	
		if (isalpha(mem[i]))
		{
			mem[i] = toupper(mem[i]);
		}
		i++;
	}

	// Display the written data
	printf("Text written to the file (%d characters):\n%s\n", 
		(int) strlen(mem), mem);

	// Unmap and close
	munmap(mem, page_size);
	close(fd);

	return 0;
}
