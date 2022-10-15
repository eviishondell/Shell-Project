#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>

//define the constants

#define CMD_HISTORY_SIZE 10

//declare the variables

char *history_cmd[CMD_HISTORY_SIZE];

int cmd_history_count = 0;

//Implement the method to execute each command of unix

static void execute_command(const char * inputline)

{
  char * command = strdup(inputline);

  char *args[10];
  
  int argc = 0;
  
  args[argc++] = strtok(command, " ");
  
  while (args[argc - 1] != NULL)
  {

    args[argc++] = strtok(NULL, " ");

  }

argc--;

int background = 0;

if (strcmp(args[argc - 1], "&") == 0) {

background = 1;

args[--argc] = NULL;

}

int fd[2] = { -1, -1 };

while (argc >= 3)

{

if (strcmp(args[argc - 2], ">") == 0)

{

fd[1] = open(args[argc - 1], O_CREAT | O_WRONLY |

O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

if (fd[1] == -1)

{

perror("open");

free(command);

return;

}

args[argc - 2] = NULL;

argc -= 2;

}

else if (strcmp(args[argc - 2], "<") == 0)

{

fd[0] = open(args[argc - 1], O_RDONLY);

if (fd[0] == -1) {

perror("open");

free(command);

return;

}

args[argc - 2] = NULL;

argc -= 2;

}

else {

break;

}

}

int status;

//create child process

pid_t pid = fork();

switch (pid) {

case -1:

perror("fork");

break;

case 0:

if (fd[0] != -1)

{

if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)

{

perror("dup2");

exit(1);

}

}

if (fd[1] != -1) {

if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {

perror("dup2");

exit(1);

}

}

execvp(args[0], args);

perror("execvp");

exit(0);

default:

close(fd[0]); close(fd[1]);

if (!background)

waitpid(pid, &status, 0);

break;

}

free(command);

}

//implement method add the commandn history after each run

static void add_to_history(const char * command) {

if (cmd_history_count == (CMD_HISTORY_SIZE - 1)) {

int i;

free(history_cmd[0]);

for (i = 1; i < cmd_history_count; i++)

history_cmd[i - 1] = history_cmd[i];

cmd_history_count--;

}

history_cmd[cmd_history_count++] = strdup(command);

}

//Implement method to run the each command

static void run_from_history(const char * command)

{

int count = 0;

if (cmd_history_count == 0)

{

printf("No commands in history\n");

return;

}

if (command[1] == '!')

count = cmd_history_count - 1;

else {

count = atoi(&command[1]) - 1;

if ((count < 0) || (count > cmd_history_count)) {

fprintf(stderr, "No such command in history.\n");

return;

}

}

printf("%s\n", command);

execute_command(history_cmd[count]);

}

//Implement the method for the history command

static void list_history()

{

int i;

for (i = cmd_history_count - 1; i >= 0; i--)

{

printf("%i %s\n", i + 1, history_cmd[i]);

}

}

//For each command signal handler

static void signal_handler(const int runcommand)

{

switch (runcommand)

{

case SIGTERM:

case SIGINT:

break;

case SIGCHLD:

while (waitpid(-1, NULL, WNOHANG) > 0);

break;

}

}

//main method
#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = ">";
char delimiters[] = " \t\r\n";
extern char **environ;



int main(int argc, char *argv[])

{
   char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];


    getcwd(command_line, MAX_COMMAND_LINE_LEN);

      
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_ARGS];
   

//This for command implementations

struct sigaction act, act_old;

act.sa_handler = signal_handler;

act.sa_flags = 0;

sigemptyset(&act.sa_mask);

if ((sigaction(SIGINT, &act, &act_old) == -1) ||

(sigaction(SIGCHLD, &act, &act_old) == -1))

{

perror("signal");

return 1;

}

size_t line_size = 100;

char * line = (char*)malloc(sizeof(char)*line_size);

if (line == NULL)

{

perror("malloc");

return 1;

}

int flag = 0;

int should_run = 1;

while (should_run)

{

if (!flag)
  printf("%s", command_line);
  printf(" ");
  printf(prompt);
  fflush(stdout);
// printf("CSC13120 > ");

// fflush(stdout);

if (getline(&line, &line_size, stdin) == -1)

{

if (errno == EINTR)

{

clearerr(stdin);

flag = 1;

continue;

}

perror("getline");

break;

}

flag = 0;

int line_len = strlen(line);

if (line_len == 1)

{

continue;

}
  #define TAM 1000 
    char buf[TAM];
    char  *gdir;
    char  *dir;
    char  *to;
// if (strcmp(argv[0], "cd")==0){

        //     gdir = getcwd(buf, sizeof(buf));
        //     dir = strcat(gdir, "/");
        //     to = strcat(dir, argv[1]);

        //     chdir(to);
        //     continue;
        //     //printf("Acceso a la carpeta realizado\n");

        // }            
  // if(argv[0] == "cd"){
  //    int chdir(const char *path);
  //  char s[100];
  //  // Printing the current working directory
  //   getcwd(s,100);
  // //  Changing the current working directory to the previous directory
  //   chdir("..");
  //  // Printing the now current working directory
  //   getcwd(s,100);
  
  // }

line[line_len - 1] = '\0';

if (strcmp(line, "exit") == 0)

{

break;

}

else if (strcmp(line, "history") == 0) {

list_history();

}

else if (line[0] == '!')

{

run_from_history(line);

}
// extern char **environ;
// else if ()

else

{

add_to_history(line);

execute_command(line);

}

}

free(line);

return 0;

}

