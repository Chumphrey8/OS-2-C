#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>/*stores the process id*/
#include <sys/wait.h>/*allows the parent process to wait in order to run the child process*/

#define MAX_LINE 80
/**
 *setup() reads in the next command line, separating it into
 *distinct tokens using whitespace as delimiters.
 *
 *setup() modifies the args parameter so that it holds pointers
 *to the null-terminated strings that are the tokens in the most
 *recent user command line as well as a NULL pointer, indicating
 *the end of the argument list.
 */
void setup(char inputBuffer[], char *args[], int *background){
    int length,/* # of characters in the command line*/
        i,/*loop index for accessing inputBuffer array*/
        start,/*index where beginning of next command parameter is*/
        ct;/*index of where to place the next parameter into args[]*/
    ct = 0; /*reads what the user entered on the command line*/
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
    start = -1;
    if(length == 0){/*if ctrl-D was entered, end of user command stream */
        exit(0);
    }
    if(length < 0){
        perror("Error reading the command");
        exit(-1);
    }/*examine every character in the inputBuffer*/
    for(i = 0; i < length; i++){
        switch(inputBuffer[i]){
            case ' ':
            case '\t':/*argument separators*/
                if (start != -1) {
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                start = -1;
                break;
            case '\n':/*newline marks the end of the command*/
                if(start != -1){
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';/*no more arguments*/
                args[ct] = NULL;
                break;
            case '&':/*command should run in the background*/
                *background = 1;
                inputBuffer[i] = '\0';
                break;
            default:/*for other characters*/
                if(start == -1){
                    start = i;
            }
        }
    }
    args[ct] = NULL;/*if the input line was greater than MAX_LINE */
}
int main(void)
{
    char inputBuffer[MAX_LINE];/*buffer to hold command entered*/
    int background;/*1 if command is followed by '&'*/
    char *args[MAX_LINE / 2 + 1];/*command line arguments*/
    while(1){ /* reset background flag for each new command*/
        background = 0;/*display the shell prompt*/
        printf("COMMAND->");
        fflush(stdout);
         /*setup() reads the user's command and
         * fills the args array.*/
        setup(inputBuffer, args, &background);
         /*fork a child process.*/
        pid_t pid = fork();
        if(pid < 0){
             /* if fork() fails.*/
            perror("Fork failed");
            exit(1);
        }
        if(pid == 0){
             /*child process.
             *execute the command entered by the user.
             *args[0] contains the command name and
             *args contains all of the command arguments.
             */
            if(execvp(args[0], args) == -1){
                perror("Command execution failed");
                exit(1);
            }
        }
        else{
             /*parent process.
             *if background == 0, the parent will wait
             * if background == 1, the parent will continue with the next iteration
             */
            if(background == 0){
                wait(NULL);
            }
        }
    }
    return 0;
}

