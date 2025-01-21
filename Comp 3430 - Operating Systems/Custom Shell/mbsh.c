// Marginal at Best Shell
//-----------------------------------------
// NAME: Ryan Dotzlaw
// STUDENT NUMBER: 7881954
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: 2
//
// REMARKS: The marginal at best shell
//
//-----------------------------------------
extern int errno;

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


typedef struct COMMAND {
	char *prog;
	int argc;
	int in_fd;
	int out_fd;
	char *fifo_path;
	char *argv[16];

} cmd;
int FIFOs = 0;
int MAX_BUF = 256;
int MAX_CMDS = 16;
int pids[128];
int pid_count = 0;


int create_cmds(cmd **arr, char **tokened, int i, int cmd_count, cmd *new_cmd);


void exec_cmds(cmd **arr, int cmd_count);

// A simple function to print out an array of strings, count elements long
void p_argv(char *args[], int count){
	int i = 0;
	while(i < count){
		printf(" * '%s'\n",args[i]);
		i++;
	}
}

// This function frees memory that was allocated to all the commands in a line
void cleanup(cmd **arr, int count){
	int i = 0;
	while(i < count){
		if (arr[i]->in_fd != -1)
			close(arr[i]->in_fd);
		if (arr[i]->out_fd != -1)
			close(arr[i]->out_fd);
		free(arr[i]);

		i++;
	}
	free(arr);
}

// This prints out the data contained in a command struct
void p_cmd(cmd *c){
	printf("%s: %d args\n", c->argv[0], c->argc);
	p_argv(c->argv, c->argc);
	printf(" * * in:%d out:%d\n",c->in_fd, c->out_fd);
}

// for debugging
void print_hex(const char *s){
	printf("0x");
	while(*s)
		printf("%02x", (unsigned int) *s++);
	printf("\n");
}

// Sets default values for a command struct, doesn't really allocate memory (it used too)
void alloc(cmd *new_cmd){

	new_cmd->in_fd = -1; //default, use stdin
	new_cmd->out_fd = -1; //default, use stdout
	new_cmd->argc = 0;
	new_cmd->fifo_path = NULL;

}

// The main part of the shell
// Takes in a line from the input file
// Parses it into an array of characters
// Loops through the array, and creates command struct instances
// to fill up the arr array
// These instances will have argc and argv values, along with in_fd and out_fd
// for later when the processes fork and execute
// the creating commands and executing parts are in helper functions
int parse2(char *line){

	// allocating memory
	cmd **arr = malloc(sizeof(cmd) * MAX_CMDS);

	// different newline characters between windows/linux causing problems
	// most likely won't cause much problems, but...
	if(line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = '\0';
	if(line[strlen(line) - 2] == '\r')
		line[strlen(line) - 2] = '\0';

	// array for storing separated line
	char *tokened[MAX_BUF];
	char *tok = strtok(line, " ");
	int i = 0;
	// split every element by spaces
	while(tok != NULL && tok[0] != '\0' && strcmp(tok, "")){
		//print_hex(tok);
		//printf("-> * * %d: \"%s\"\n",i, tok);
		tokened[i] = tok;
		tok = strtok(NULL, " "); // continue splitting
		i++;
	}

	// im mostly convinced this is only needed due to the windows/linux new line diff
	if(i == 0 && (tok == NULL || tok[0] == '\0' || strcmp(tok, "")))
		return -1;

	//p_argv(tokened);

	// format goes: <command name> <arg1> ... <argn> <input> <output/pipe>
	// args can start w/ '-' or they can be file names, assume everything is an arg until
	// you reach an input or output or end of the line

	int cmd_count = 0;

	// while there's still tokens to parse
	cmd *new_cmd = malloc(sizeof(cmd));
	alloc(new_cmd);

	// create all commands and fill arr, along with returning the number of commands created
	cmd_count = create_cmds(arr, tokened, i, cmd_count, new_cmd);


	// run all the commands for the current line
	exec_cmds(arr, cmd_count);

	return -1;


}

// this executes all the commands located in the arr array of length cmd_count
void exec_cmds(cmd **arr, int cmd_count) {
	int x = 0;
	while(x <= cmd_count) {
		//printf("forkin' fantastic\n");
		int pid = fork();
		if (pid == 0) {
			// in child

			// print out command to stdin for debug
			/*
			printf("%s\n", arr[x]->prog);

			printf("%d\n", arr[x]->argc);
			printf("%d\n", arr[x]->in_fd);
			printf("%d\n", arr[x]->out_fd);
			 */
			//printf("%s\n", arr[x]->argv[0]);
			//printf("%s\n", arr[x]->argv[1]);
			//printf("child dupin'\n");

			// set input/output streams
			if (arr[x]->in_fd != -1) {
				close(0);
				dup2(arr[x]->in_fd, STDIN_FILENO);

			}
			if (arr[x]->out_fd != -1) {
				close(1);
				dup2(arr[x]->out_fd, STDOUT_FILENO);
			}

			// exec program
			execvp(arr[x]->argv[0], arr[x]->argv);

			exit(EXIT_SUCCESS);
		} else if(pid > 0){
			printf("Executing %s on pid %d\n", arr[x]->argv[0], pid);
			pids[pid_count] = pid;
			pid_count++;

			p_cmd(arr[x]);
			//wait(NULL);
		} else {
			printf("Failure to create thread\n");
		}
		x++;
	}

	// all commands in arr are now executed
	// arr is no longer needed
	cleanup(arr, cmd_count);
}

// This creates the commands that need to be executed in a single line

// This returns the number of commands created
int create_cmds(cmd **arr, char **tokened, int i, int cmd_count, cmd *new_cmd) {
	int j = 0;
	while(j < i){

		//printf("%d: %d->%s\n", j, cmd_count, tokened[j]);
		//printf("cmd #%d\n", cmd_count);
		if(new_cmd->argc == 0){
			// first arg == command
			new_cmd->argc = 1;
			new_cmd->argv[0] = tokened[j];
			new_cmd->prog = tokened[j];
			arr[cmd_count] = new_cmd;
			//cmd_count++;
		} else if(tokened[j] != NULL){
			//printf("->non null\n");
			if(!strcmp(tokened[j],">")){
				//printf("->output to file\n");

				//printf("%d:%s, %d:%s\n",j, tokened[j],j+1, tokened[j+1]);

				// output to file
				new_cmd->out_fd = open(tokened[j+1], O_WRONLY|O_CREAT|O_TRUNC, 0777);
				// if reached output, then new_cmd is done...
				if(new_cmd->out_fd == -1)
					printf("failed to open write end for %s\n", tokened[j+1]);
				// skip over filename
				j++;
			} else if(!strcmp(tokened[j],"<")){
				//printf("->input from file\n");
				new_cmd->in_fd = open(tokened[j+1], O_RDONLY, 0777);
				// skip over filename, already got it
				if(new_cmd->in_fd == -1)
					printf("failed to open read end for %s\n", tokened[j+1]);
				j++;
			} else if(!strcmp(tokened[j],"|")){
				// piping previous command into next command

				// create new command

				//printf("create new cmd\n");
				new_cmd = malloc(sizeof(cmd));
				alloc(new_cmd);
				//printf("put cmd in array\n");
				arr[cmd_count+1] = new_cmd;
				//printf("create pipe\n");
				// create pipe
				int pipefd[2];
				pipe(pipefd); // if theres an error, we're screwed
				//printf("set pipe\n");
				// set inputs for commands
				new_cmd->in_fd = pipefd[0];
				//printf("done pipe1\n");


				//printf("%s %s\n", arr[0]->prog, arr[1]->prog);
				arr[cmd_count]->out_fd = pipefd[1];

				//printf("done pipe2\n");
				cmd_count++;


			} else if(tokened[j][0] == '<' && tokened[j][1] == '('){
				//printf("doing proc sub\n");
				// process substitution
				// these can be assumed to be SIMPLE (no redirection or pipes in them)
				// -> AKA: there is only arguments inside
				new_cmd = malloc(sizeof(cmd));
				alloc(new_cmd);
				arr[cmd_count+1] = new_cmd;
				char *com = malloc(64);
				// copy over all chars, except the '<' and '(' chars
				unsigned long x = 2;
				while(x < strlen(tokened[j])){
					com[x-2] = tokened[j][x];
					com[x-1] = '\0';
					x++;
				}
				j++; // increment j (go to next argument)

				new_cmd->prog = com; // mem was allocated prev, so it's all good
				new_cmd->argc = 1;
				new_cmd->argv[0] = com;


				// take arguments until the last argument, which will end in
				while(tokened[j][strlen(tokened[j]) - 1] != ')'){
					new_cmd->argv[new_cmd->argc] = tokened[j];
					new_cmd->argc++;
					j++;
				}

				tokened[j][strlen(tokened[j]) - 1] = '\0'; // delete last char, the ')'
				new_cmd->argv[new_cmd->argc] = tokened[j];
				new_cmd->argc++;


				// done setting up p sub command

				int pipefd[2];
				pipe(pipefd);

				/* mkfifo wasn't working, like at all,
				 * (opening the fifo to get the read fd made it block wait, and if i set open flag to O_NONBLOCK
				 * then once it tried to use it, it threw a fit
				 * and then I thought, ain't p sub basically just pipes but w/ command order reversed?
				*/


				new_cmd->out_fd = pipefd[1];
				//new_cmd->fifo_path = name;
				arr[cmd_count]->in_fd = pipefd[0]; // this is the previous command
				//arr[cmd_count]->fifo_path = name;

				// subbed process runs before the other
				// ie, in : head -5 <(sort -R words)
				// sort -R words performs a random sort on the words
				// then head -5 reads in the sorted words and takes the first 5
				// --> SORT RUNS FIRST?
				cmd *temp = arr[cmd_count];
				arr[cmd_count] = new_cmd;
				arr[cmd_count + 1] = temp;
				// the proc sub is over, it has no more commands
				// but the process its subbing into can still have an output
				new_cmd = temp;

				cmd_count++;

			} else {
				//printf("i:%d j:%d\n",i,j);
				//if(j > i) // this needs to be here because... i have no idea
				//	break;

				// is an argument
				//new_cmd->argv[new_cmd->argc] = malloc(64);
				new_cmd->argv[new_cmd->argc] = tokened[j]; // don't copy address, copy data
				new_cmd->argc++;
				//printf(" (*) argc:%d j:%d %s\n",new_cmd->argc, j, tokened[j]);


			}
		}
		j++;
	}
	return cmd_count;
}


int main(int argc, char* argv[]){
	argc = -1; // gets rid of unused var warning (i aint usin this)
	// open the file
	FILE *f = fopen(argv[1], "r");
	// allocate buffer
	char *buf = malloc(MAX_BUF);
	// repeatedly read in lines
	while(fgets(buf, MAX_BUF, f)){
		printf("'%s'", buf);
		//int pid =
		if(buf != NULL && buf[0] != '\0' && strcmp(buf, "") && buf[0] != '\r' && buf[0] != '\n')
			parse2(buf);
		wait(NULL);

	}
	printf("All commands read and executed...\n");

	// do cleanup
	for(int i = 0; i < pid_count; i++){
		// just try and kill all child processes
		kill(pids[i], SIGKILL);

	}


	free(buf);
	fclose(f);
	return EXIT_SUCCESS;
}
