#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void handle_sigstp(int);
int perform_Task(char* [512], int);
int shell_Loop();

int exit_stat = 0;
int process_ammount = 0;
int all_Process[1000];
struct sigaction ctrc;
struct sigaction ctrz;
int background = 0;
int foreground_only = 0;

/*---------BEGIN PROGRAM---------*/


int main() {
    ctrc.sa_handler = SIG_IGN; //handle sigint
    ctrc.sa_flags = SA_RESTART;
    sigfillset(&ctrc.sa_mask);
    sigaction(SIGINT, &ctrc, NULL);

    ctrz.sa_handler = &handle_sigstp; //handle sigstp
    sigfillset(&ctrz.sa_mask);
    sigaction(SIGTSTP, &ctrz, NULL);


    int exit = 1;
    while (exit == 1) {
        exit = shell_Loop (); //loop until exit
    }

    return 0;
}

void kill_P () {
    for (int i = 0; i < process_ammount; i++) { //kill all processes
            kill(all_Process[i], SIGTERM);
    }
}

void handle_sigstp(int sig) {
    if (foreground_only == 0) { //print message if not in foreground on press of ctrl z
        write(STDOUT_FILENO, "\nEntering foreground-only mode (& is now ignored)\n", 50);
        foreground_only = 1;
    }
    else {//print message if in foreground on press of ctrl z
        write(STDOUT_FILENO, "\nExiting foreground-only mode\n", 30);
        foreground_only = 0;
    }
    
}

int execute_Other(char* arguments[512], int arg) { //execute any other command
    pid_t pid = fork(); //make a child process
    int searched_pid;
    all_Process[process_ammount] = pid; //store the child pid

    switch(pid) {
        case -1:
            printf("Error with creatiion\n");
            fflush(stdout);
            break;
        case 0: //child proccess
            if (background == 0) {
                ctrc.sa_handler = SIG_DFL;
                sigaction(SIGINT, &ctrc, NULL);
            } //ignore ctrl c

            for (int i = 0; i < arg; i++) { 
                if (strcmp(arguments[i], "<") == 0){ //check for input
                    arguments[i] = NULL; //removve the operator
                    int file_in = open(arguments[i + 1], O_RDONLY); //open file
                    if (file_in == -1) { //see if exist
                        printf("File doesn't Exist\n");
                        fflush(stdout); 
                        exit(1);
                    }
                    dup2(file_in, STDIN_FILENO);
                    close(file_in);
                }
                else if (strcmp(arguments[i], ">") == 0){ //check for output
                    arguments[i] = NULL; //removve the operator
                    int file_out = open(arguments[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777); //open file
                    dup2(file_out, STDOUT_FILENO);
                    close(file_out);
                }
            }
            
            if(execvp(arguments[0], arguments) == -1) { //perform the command
                printf("Error running %s\n", arguments[0]); //if error print and exit
                fflush(stdout); 
                exit(1);
            }
            break; 
        default: 
            if (background == 1 && foreground_only == 0) { //run in backgrounf only if foreground process mode not enabled
                waitpid(pid, exit_stat, WNOHANG);
                printf("Background process has started. pid: %d\n", pid);
                fflush(stdout);
            }
            else {
                waitpid(pid, exit_stat, 0); //wait for child to finish
            }

        while ((searched_pid = waitpid(-1, exit_stat, WNOHANG)) > 0) { //check if child done
                    printf("pid: %d has completed\n", searched_pid);
                    //print exit value
                    fflush(stdout);
                }    
    }
    return 1;
}

int perform_Task(char* arguments[512], int total_arg) {
    
    if (arguments[0][0] == '#') { //if we have a # ignore
        return 1;
    }
    else if (strcmp(arguments[0], "exit") == 0) {
        kill_P();
        return 0;
    }
    else if (strcmp(arguments[0], "cd") == 0) { //execute if cd is the ctrc argument
        if (total_arg == 1) { //if one argument go home
            chdir(getenv("HOME"));
        }
        else if (total_arg == 2) { //if two arguments change directory to argument 2
            int valid = chdir(arguments[1]); //check if valid directory
            if (valid == -1) {
                printf("Directory doesn't exist.\n"); //print error if directory does not exist
                fflush(stdout);
            }
        }
        return 1;
    }
    else if (strcmp(arguments[0], "status") == 0) { //status command
        if (WIFEXITED(exit_stat)) {
            printf("exit value %d\n", WEXITSTATUS(exit_stat));
        }
        return 1;
    }
    else {
        return execute_Other(arguments, total_arg); //execute any other cmomand
    }
}

int shell_Loop(int exit_stat) {
    char line[2048]; //string for user input
    char* arguments[512]; //array of arguments

    printf(": ");
    fflush(stdout); 

    fgets(line, 2048, stdin); //get user input
    line[strcspn(line, "\n")] = '\0'; //remove for comparing

    if (line[0] == '\0') { //check if there is no input (empty)
        return 1;
    }

    int i = 0; //start at 0th argument in array
    char* token = strtok (line, " "); //go to spaces
        while (token != NULL) { //go until last argument
            arguments[i] = strdup(token); //save token into the argument array

            for (int x = 0; x < strlen(arguments[i]); x++) { //expansion on $$
                if (arguments[i][x] == '$' && x != (strlen(arguments[i]) - 1) && arguments[i][x + 1] == '$') {
                    arguments[i][x] = '\0';
                    snprintf(arguments[i], 400, "%s%d", arguments[i], getpid());
                }
            }
            
            token = strtok(NULL, " "); //get the next token
            i++; //iterate through array
        }

        arguments[i] = NULL; //set it null for executing other comands

        if (strcmp(arguments[i - 1], "&") == 0) {
            i--;
            arguments[i] = NULL;
            background = 1; //if there is an & at the end then set background
        }else {
            background = 0; //no & dont set
        }   

        return perform_Task(arguments, i); //return to main to see if exit or not
}
