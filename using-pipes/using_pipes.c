
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main(int argc, char** argv)
{
     // Pipes
     // pipe1 is from source to filter, pipe2 is from filter to sink
     int pipe1[2], pipe2[2];
     
     // Create pipes
     if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
     {
          perror("Creating pipes failed!");
     }

     // Fork the source process (ls)
     if (fork() == 0)
     {
          // Redirect the stdout to the write end of pipe1
          close(1);
          dup(pipe1[1]);
          close(pipe1[0]);

          close(pipe2[0]);
          close(pipe2[1]);

          execlp("ls", "ls", NULL);
          exit(0);
     }
     else
     {
          // Fork the filter process (sort -r)
          if (fork() == 0)
          {
               // Redirect the read end of pipe1 to stdin
               close(0);
               dup(pipe1[0]);
               close(pipe1[1]);

               // Redirect the stdout to the write end of pipe2
               close(1);
               dup(pipe2[1]);
               close(pipe2[0]);

               execlp("sort", "sort", "-r", NULL);
               exit(0);
          }
          else
          {
               // Fork the sink process (more)
               if (fork() == 0)
               {
                    close(pipe1[0]);
                    close(pipe1[1]);

                    // Redirect the read end of pipe2 to stdin
                    close(0);
                    dup(pipe2[0]);
                    close(pipe2[1]);

                    execlp("more", "more", NULL);
                    exit(0);
               }
          }
     }

     // Close unused pipes
     close(pipe1[0]);
     close(pipe1[1]);

     close(pipe2[0]);
     close(pipe2[1]);

     // Wait for all childs to terminate
     pid_t pid;
     while (pid = waitpid(-1, NULL, 0)) 
     {
          if (errno == ECHILD) 
          {
               break;
          }
     }

     return 0;
}
