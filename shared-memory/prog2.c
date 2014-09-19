#include <string.h>
#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <ctype.h>

#define SHSIZE 500

int main(int argc,char *argv[])
{

	printf("Program 2: shm1 --> konverterer --> shm2\n");

	int shmid1;
	int shmid2;
	key_t key = 1234;
	key_t key1 = 4321;	
	char *shm1;
	char *shm2;
	char *s;
	char buffer[150];

	//Laver id til shm1
	shmid1 = shmget(key, SHSIZE, 0666);
	if(shmid1 < 0)
	{
		perror("shmget");
		exit(1);
	}
	//mounter shm1
	shm1 = shmat(shmid1, NULL, 0);
	if(shm1 == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}
	//finder laengde paa data i shm1
	int i;
	for(s = shm1; *s != 0; s++)
	{
		i++;		
	}
	
	// copy data til shm1
	memcpy(buffer, shm1, i);
   	// sletter shm1 og unmounter
   	shmctl(shmid1, IPC_RMID, 0);
   	//unmounter shm1
   	shmdt(shm1);
	
   	// Konverterer bufferens data fra lowercase til uppercase
   	int o;
   	for(o=0; o < i ;o++)
   	{
        buffer[o] = toupper(buffer[o]);    
  	}
  	
	//Skaber og laver id til shm2  
	shmid2 = shmget(key1, SHSIZE, IPC_CREAT | 0666);
	if(shmid2 < 0)
	{
		perror("shmget");
		exit(1);
	}
	
	//mounter til shm2
	shm2 = shmat(shmid2, NULL, 0);

	if(shm2 == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}

	//copy data fra buffer til shm2
	memcpy(shm2, buffer, strlen(buffer));	
	
	//unmounter shm2
	shmdt(shm2);
	
	exit(0);
}