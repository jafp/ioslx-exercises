#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SHSIZE 500  
 
int main(int argc,char *argv[])
{
  
  pid_t pid1 , pid2 , pid3;
  int status1;
  int status2;
  int status3;
  int pidstatus1;
  int pidstatus2;
  int count = 1;
  int linecount_id;
  char *linecount;
  key_t key;
  key = 5554;

  
  // laver delthukommelse til linjeinformation
  linecount_id = shmget(key, SHSIZE, IPC_CREAT | 0666);
  
  if(linecount_id < 0)
  {
    perror("in start shmget");
    exit(1);
  }

  linecount = shmat(linecount_id, NULL, 0);

  if(linecount == (char *) -1)
  {
    perror("shmat");
    exit(1);
  }
 // sletter output filen hvis den er der, giver fejl foerste gang programmet koeres, da der ikke er noget output file endnu
  FILE *fp;
    
  static int filestatus;

  filestatus = remove("output.txt");
 
   if( filestatus == 0 )
      printf("Output filen er blevet slettet.\n");
   else
   {
      printf("Kunne ikke slette filen \n");
      perror("Error");
   }  
    // AAbner input filen for at faa information om hvor mange linjer filen indeholder 
    fp = fopen("input.txt","r");
    if (fp == NULL)
    {
        perror("start : Can not open file !!");
        exit(EXIT_FAILURE);
    }

    static int c;       
    unsigned long newline_count = 0;

    //taeller linjer i inputtxt.txt filen
    while ( (c=fgetc(fp)) != EOF ) {
        if ( c == '\n' )
            newline_count++;
    }
    //lukker intput file
    fclose(fp);
  
// Dette while-loop koerer saa laenge der er nye linier i input filen
//while(count != newline_count +1)
while(count <= newline_count)
{
    
    //opdaterer den delte hukommelse med hvilken linje i input filen programmet er kommet til
    *linecount = count;

    // forker en ny process 1
    pid1 = fork();
   
    if (pid1 == -1)
    {
      fprintf(stderr, "can't fork, error %d\n", errno);
      exit(EXIT_FAILURE);
    }
    else if (pid1 == 0)
    {    
      // starter eksternt program
      execl("prog1","prog1",NULL);           
    }
   
    else
    {  
      // Venter paa process 1
      waitpid(pid1, &status1 , 0);   

      //forker en ny process 2
      pid2 = fork();
      if (pid2 == -1)
      {
        fprintf(stderr, "can't fork, error %d\n", errno);
        exit(EXIT_FAILURE);
      }

      else if(pid2 == 0)
      {
        //starter eksternt program  
        execl("prog2","prog2",NULL);
      }
    
      else
      {
        // venter paa process 2 
        waitpid(pid2, &status2, 0);
        //forker en ny process 3
        pid3 = fork();
      
        if (pid3 == -1)
       {
          fprintf(stderr, "can't fork, error %d\n", errno);
          exit(EXIT_FAILURE);
        }

        else if(pid3 == 0)
        {
          execl("prog3","prog3",NULL);
          
        }
        else
        {
          waitpid(pid3, &status3, 0);
        }
      }
    } 
    // taeller count op 
    count++;
  }
  //sletter delt hukommelse ("linecount")
  shmctl(linecount_id, IPC_RMID, 0);
  //unmounter delt hukommelse ("linecount")  
  shmdt(linecount);
  return 0;
}