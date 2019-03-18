#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

/* Brian Dinh's Silly Shell */

int backgroundStatus;

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
        if(backgroundStatus == 0) // if background status is false (0), then the parent execution will be on hold until child process has changed state
        {
            if (waitpid(pid, &status, 0) > 0) // waitpid() puts parent execution on hold until child process has changed state
            {
                if (WIFEXITED(status) && !WEXITSTATUS(status)) // detects if the child process has returned a successful state
                {
                    printf("\n%s executed successfully\n\n", *argv);
                }
                else if (WIFEXITED(status) && WEXITSTATUS(status))
                {
                    if (WEXITSTATUS(status) == 127) // detects if the execvp system call fails
                    {
                        printf("\n%s failed\n\n", *argv);
                    }
                    else // detects that if the child process returns a non-zero status
                    {
                        printf("\n%s terminated normally, but returned a non-zero status\n\n", *argv);
                    }
                }
                else // detects that the child process does not terminate regularly
                {
                    printf("\n%s didn't terminate normally\n\n", *argv);
                }
                
            }
            else // detects if the waitpid() system call fails
            {
                printf("waitpid() failed\n\n");
            }
            
        }
        else
        {  /* *** if background status is not equal to 0 (it is equal to 1), do NOT wait for the child, resume normal execution. *** */
            backgroundStatus = 0; // reset backgroundStatus back to false for more checks
        }
    }
}

void printenv(char **envp) // prints out all environment variables to the shell by
{
    char *substring_pointer;
    
    printf("Print all env variables containing SHELL in the name\n");
    
    while (*envp != NULL)
    {
        substring_pointer = strstr(*envp, "SHELL"); // gets information from SHELL only
        
        if (substring_pointer != NULL) printf("%s\n", *envp);
        
        envp++;
    }
    
    printf("Well, that just about does it.\n\n");
}

void doNothing(int signum){ /* Do nothing if Ctrl-C is detected */ }

int main(int argc, char **argv, char **envp)
{
    char line[1024];
    char *largv[64];
    char shell_prompt[33];
    
    strcpy(shell_prompt, "BrianDinhsSuperAwesomeSillyShell"); // give my shell a super awesome name
    
    signal(SIGINT, doNothing); // attaches ignore signal for ctrl-c
    
    while (1)
    {
        printf("%s> ",shell_prompt);
        
        if (fgets(line, 1024, stdin) == NULL) // if fgets routine returns NULL, we know that the user inputed ctrl-d, therefore quit the program when this detection occurs
        {
            printf("Detected Ctrl-d, Good-bye!\n");
            exit(0);
        }
        else
        {
            line[strlen(line)-1]='\0';
    
            if (*line != '\0')
            {
                parse(line, largv);
                
                if (strcmp(largv[0], "exit") == 0 || strcmp(largv[0], "done") == 0 || strcmp(largv[0], "quit") == 0) exit(0); else
                    if (strcmp(largv[0], "printenv")  == 0) printenv(envp); else
                        if (strcmp(largv[0], "newprompt") == 0)
                        {
                            if (largv[1] != NULL)
                                strncpy(shell_prompt, largv[1], 33);
                            else
                                strncpy(shell_prompt, "BrianDinhsSuperAwesomeSillyShell", 33);
                        }
                        else
                        {
                            if(largv[1] != NULL)
                            {
                                if (strcmp(largv[1], "&") == 0) // if the second token is &, set "boolean" backgroundStatus to be true (1)
                                {
                                    backgroundStatus = 1;
                                }
                                else // if second parameter is not &, set "boolean" backgroundStatus to be false (0)
                                {
                                    backgroundStatus = 0;
                                }
                            }
                            
                            execute(largv);
                        }
            }
        }
    }
}

                

