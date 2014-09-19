
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

	// Open file in readyonly mode.
	fd = open("mem_mapped_file.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		exit(1);
	}

	// Map file to memory
	mem = mmap((caddr_t) 0, page_size, PROT_READ, 
		MAP_SHARED, fd, 0);

	if (mem == (caddr_t) -1)
	{
		perror("mmap");
		exit(1);
	}

	// Print the text in the file
	printf("Text read from file (%d characters):\n%s\n", 
		(int) strlen(mem), mem);

	// Show random access by printing a specific position
	printf("Random access. Character at pos 3: %c\n", mem[3]);

	munmap(mem, page_size);
	close(fd);

	return 0;
}
