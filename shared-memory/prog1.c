#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#define SHSIZE 500

int main(int argc,char *argv[])
{
	printf("Program 1: input ---> shm1\n");
	
	
    FILE  *fp;
	int shmid1;
	int linecountid;
	key_t key = 1234;
	key_t key2 = 5554;	
	char *shm1;
	char *linecount;
	char *s;
		
	//Laver id til den delte hukommelse
	linecountid = shmget(key2, SHSIZE, 0666);
	if(linecountid < 0)
	{
		perror("shmget");
		exit(1);
	}	
	// skaber forbindelse til delt hukommelse hvor linjeinformationen ligger.	
	linecount = shmat (linecountid, NULL, 0);
	if(linecount == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}


    // laver delt hukommelse id til shm1
	shmid1 = shmget(key, SHSIZE, IPC_CREAT | 0666);
	if(shmid1 < 0)
	{
		perror("shmget");
		exit(1);
	}
	// mounter den delte hukommelse shm1
	shm1 = shmat (shmid1, NULL, 0);
	if(shm1 == (char *) -1)
	{
		perror("shmat");
		exit(1);
	}	
	//aabner indput file 
	fp = fopen("input.txt","r");
	if (fp == NULL)
    {
        perror(" porg 1 : Can not open file !!");
        exit(EXIT_FAILURE);
    }
	
    int line = 1;//Start linje
    int found = 0; 
    char liner[400];//bufffer til data
    int linesearch = *linecount;//Hvilken linje vi gerne vil laese. Kommer fra den delte hukommelse("linecount")
    
    // Finder den oenskede linje i input filen og skriver den til shm1
    while(fgets(liner,40,fp))
    {
        if(linesearch == line)
            {
                found = 1;
                //copy buffer til shm1
                memcpy(shm1, liner, strlen(liner));                             
            }
                                  
        line++; // taeller linjen op                                
    }
       
    if (found == 0)
    {            
        printf("Linie nummer %d findes desvaerre ikke!",linesearch);             
    }
    
    line = line - 1;
	//lukker input file  
	fclose(fp);
	//unmounter shm1
	shmdt(shm1);
	exit(0);

}


   