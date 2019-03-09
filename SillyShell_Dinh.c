#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Brian Dinh's Silly Shell */

void parse(char *line, char **argv)
{
     while (*line != '\0')
        {
          while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r')
           {
               *line = '\0';
               line++;
           }
            
          *argv++ = line;

          while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n' && *line !='\r')
          {
              line++;
          }
        }
    
     *argv = NULL;
}

void execute(char **argv)
{
    pid_t pid;
    
    int ret = 1;
    int status;
    
    pid = fork(); // creates child process
    
    if (pid == -1) // forking error is detected
    {
        printf("Fork Error\n");
    }
    else if (pid == 0) // if pid of 0, then it is the child process
    {
        execvp(*argv, argv); // the child process will rewrite its text segment to match the external command. it will pass right amount of parameters whether it is 0 parameters or n parameters
    }
    else // if pid returned is not -1 (error) or 0 (child process), then it is the parent process
    {
        if (waitpid(pid, &status, 0) > 0) // waitpid() puts parent execution on hold until child process has changed state
        {
            if (WIFEXITED(status) && !WEXITSTATUS(status)) // detects if the child process has returned a successful state
            {
                printf("\n%s executed successfully\n\n", *argv);
            }
            else if (WIFEXITED(status) && WEXITSTATUS(status))
            {
                if (WEXITSTATUS(status) == 127) // if
                {
                    printf("\n%s failed\n\n", *argv); // execv failed
                }
                else
                {
                    printf("\n%s terminated normally, but returned a non-zero status\n\n", *argv);
                }
            }
            else
            {
                printf("\n%s didn't terminate normally\n\n", *argv);
            }

        }
        else 
        {
            printf("waitpid() failed\n\n");
        }
    }
}

void printenv(char **envp)
{
    printf("Let's print out all the environment variables\n\n");
    
    while (*envp != NULL)
    {
        printf("%s\n", *envp);
        envp++;
    }
    
    printf("Well, that just about does it.\n\n");
}


int main(int argc, char **argv, char **envp)
{
     char line[1024];
     char *largv[64];
     char shell_prompt[16];

     strcpy(shell_prompt, "DinhsSillyShell");
    
     while (1)
     {
         printf("%s> ",shell_prompt);
         
         fgets(line, 1024, stdin);
         
         line[strlen(line)-1]='\0';
         
         if (*line != '\0')
         {
             parse(line, largv);
             
             if (strcmp(largv[0], "exit") == 0 || strcmp(largv[0], "done") == 0 || strcmp(largv[0], "quit") == 0) exit(0); else
                 if (strcmp(largv[0], "printenv")  == 0) printenv(envp); else
                     if (strcmp(largv[0], "newprompt") == 0)
                     {
                         if (largv[1] != NULL)
                             strncpy(shell_prompt, largv[1], 16);
                         else
                             strncpy(shell_prompt, "DinhsSillyShell", 16);
                     }
                     else
                     {
                         execute(largv);
                     }
         }
     }
}

                

