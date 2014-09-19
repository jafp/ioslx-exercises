#include <string.h>
#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
 
#define SHSIZE 500

int main(int argc,char *argv[])
{

	printf("Program 3: shm2 ---> sink ---> output fil\n");
	printf("\n");

	FILE *fp;
	int shmid2;
	key_t key2;
	char *shm2;
	char *s;

	key2 = 4321;

	// Laver id til delt hukommelse shm2
	shmid2 = shmget(key2, SHSIZE, 0666);
	if(shmid2 < 0)
	{
		perror("shmget");
		exit(1);
	}
	//mounter delt hukommelse shm2
	shm2 = shmat (shmid2, NULL, 0);

	if(shm2 == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}

	//aabner output filen
	fp = fopen("output.txt","a+");
	
	//skriver til output filen
	fputs(shm2, fp);
	
	//Lukker output filen
	fclose(fp);
	
	//sletter shm2
	shmctl(shmid2, IPC_RMID, 0);
	
	//unmounter shm2
	shmdt(shm2);


	exit(0);

}