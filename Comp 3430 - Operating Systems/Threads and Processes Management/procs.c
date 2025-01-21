
//-----------------------------------------
// NAME: Ryan Dotzlaw
// COURSE: COMP 3430, SECTION: A02
// ASSIGNMENT: 1, QUESTION: 2a
//
// REMARKS: Managing processes and communication with signals
//
//-----------------------------------------

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

//-----------------------------------------
// Function declarations
//-----------------------------------------

// A function for reading "config.txt"
void config();
// A function to update the number of workers
void update_w();
// A function to create a single worker and add it to the end of the workers array
void create_w();
// A function to remove a single worker from the end of the workers array
void remove_w();
// A function to keep the workers busy
void work();
// A function that is called by workers when they're cleaning up and exiting
void worker_clean();
// A function called by the handler when it's time to clean up all workers and shutdown
void handler_clean();

//-----------------------------------------
// Data declarations
//-----------------------------------------

typedef struct HANDLER {
	// The maximum number of workers, or rather, the number of workers to have
	int num_workers;
	// The current number of workers running (length of workers array)
	// If this is different from num_workers, need to add/remove workers
	int cur_workers;
	// A list of all processes
	pid_t workers[];
} handler;

// Config filename
static char* FILE_NAME = "config.txt";

// Initialize handler struct
handler h;

//-----------------------------------------
// Function definitions
//-----------------------------------------

//------------------------------------------------------
// config
// * Only for use by the handler
// PURPOSE: Reads the file "config.txt" when called and updates the
//			value: 'num_workers' in the 'h' struct.
//			Then calls 'update_w'
//------------------------------------------------------
void config(){
	printf(" * Beginning to read config file...\n");

	int buf;

	// open config in read only mode
	FILE *f = fopen(FILE_NAME, "r");

	// read in the number of workers
	// can be in range [0,9], simply because I don't want to deal w/ converting a string to an int or whatever
	buf = fgetc(f) - '0'; // - '0' converts char number to int number
	printf(" * New number of workers: %d\n", buf);
	h.num_workers = buf;

	fclose(f);
	printf(" * Done reading config file.\n");
	update_w();
	// Done reading config and updating
	// handler can return to whatever it was doing before interrupt
}

//------------------------------------------------------
// update_w
// * Only for use by the handler
// PURPOSE: Uses the handler struct 'h', and it's associated
//			values: 'num_workers' and 'cur_workers' to decide
//			whether to create/remove workers and how many.
//------------------------------------------------------
void update_w(){

	printf(" * * Beginning to update workers\n");
	int difference = h.num_workers - h.cur_workers;
	// for output
	int d = abs(difference);
	if(difference == 0){ // No difference, no need for updates
		printf(" * * No new workers were created\n");
		// Handler goes back to idling

	} else if(difference > 0){ // difference positive, need to create more workers

		// create 'difference' more workers
		while(difference != 0){
			create_w();
			difference--;
		}
		printf(" * * %d new workers were created.\n",d);

	} else { // difference negative, need to remove workers

		// remove 'abs(difference)' workers
		while(difference != 0){
			remove_w();
			difference++;
		}
		printf(" * * %d workers were removed.\n",d);

	}
	// All changes are done, handler will return to config function call
}

//------------------------------------------------------
// create_w
// * Only for use by the handler
// PURPOSE: A helper function for 'update_w', creates a single worker
//			and appends it to the end of the 'workers' array in the 'h' struct.
//------------------------------------------------------
void create_w(){

	// Create a child process
	pid_t f = fork();

	if(f == 0){ // In the child

		// Setup signal handlers
		// when worker receives SIGUSR1, clean up and exit
		signal(SIGUSR1, worker_clean);
		// Work until signal is received
		printf(" * * * Worker #%d created and beginning to work.\n", getpid());
		work();

	} else if(f > 0){ // In the parent

		// remember the child's pid for later
		h.workers[h.cur_workers] = f;
		h.cur_workers++;

	} else { // fork failed!
		// if the fork fails, honestly just give up
		// most likely fails due to not having enough resources, so try and cleanly exit
		printf("An error has occurred...\n");
		printf(" * * * Failed to create a worker...\n");
		printf("Program trying to cleanup...\n");
		handler_clean(); // maybe will work, maybe not

	}

}

//------------------------------------------------------
// remove_w
// * Only for use by the handler
// PURPOSE: A helper function for 'update_w', removes a single worker
//			from the end of the 'workers' array in the 'h' struct.
//------------------------------------------------------
void remove_w(){

	// grab pid of worker about to be removed
	int pid = h.workers[h.cur_workers - 1];

	// send signal to last worker, causing it to clean up
	kill(h.workers[h.cur_workers - 1], SIGUSR1);

	// wait for exit of worker
	// should be skipped if worker already exited
	sleep(1);
	waitpid(pid, NULL, 2);

	// clear pid from list
	h.workers[h.cur_workers - 1] = 0;
	// decrement cur_workers
	h.cur_workers--;
	// done

}

//------------------------------------------------------
// work
//
// PURPOSE: Keeps a process 'busy', if the process doesn't receive a
//			signal, it will be forever stuck in this function.
//------------------------------------------------------
void work(){

	while(1){
		sleep(1);
	}

}

void worker_clean(){

	printf(" * * * Worker #%d clean up and exiting...\n", getpid());
	fflush(stdout);
	exit(EXIT_SUCCESS);

}

void handler_clean(){

	printf(" Beginning total cleanup.\n");
	pid_t pid = h.workers[0];
	int status;
	// iterate though list of worker pid's, removing them
	while(h.cur_workers != 0){
		// remove worker
		remove_w();

		// continue loop
		// * h.cur_workers is decremented in remove_w, so no need to change loop var
	}
	// wait for final worker and grab it's status
	waitpid(pid, &status, 0);

	// WIFEXITED takes the status from a wait call, if the status said the process has exited successfully,
	// then WIFEXITED will return true, else false
	// this is here because sometimes workers won't signal that they're exiting if the handler receives an interrupt signal
	// they do it if they need to exit while the handler is still running, but not here
	if(WIFEXITED(status)){
		printf(" * All workers have exited, handler exiting.\n");
		printf("Program completed successfully.\n");
		exit(EXIT_SUCCESS);
	} else {
		printf(" * Not all workers have exited, handler exiting.\n");
		exit(EXIT_FAILURE);
	}


}

//-----------------------------------------
// Main definition
//-----------------------------------------

//------------------------------------------------------
// main
//
// PURPOSE: Performs initial program setup and handler setup
//------------------------------------------------------

int main(){

	printf("Program starting...\n");
	printf("%d is the handler.\n", getpid());
	// Firstly, need to read the config file to know how many workers to create initially
	config();
	// config calls the update_w function, so workers are created as necessary

	// this will never be reached by child processes, if it is, something is terribly wrong

	// set up handler signal listeners
	// the handler interrupt signal, clean up all child processes, then exit
	signal(SIGINT, handler_clean);
	// the signal to re-read config file
	signal(SIGHUP, config);

	printf(" * Initial setup complete!\n");
	// keep handler busy, waiting for signals
	work();

	printf("This shouldn't have been reached...\n");
	printf("Program trying to cleanup...\n");
	handler_clean(); // maybe will work, maybe not

}
