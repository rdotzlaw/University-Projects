//-----------------------------------------
// NAME: Ryan Dotzlaw
// COURSE: COMP 3430, SECTION: A02
// ASSIGNMENT: 1, QUESTION: 2b
//
// REMARKS: Managing threads and communication through shared memory
//
//-----------------------------------------

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>


//-----------------------------------------
// Data declarations
//-----------------------------------------

typedef struct THREAD {
	pthread_t p;
	int signal; // signal = 1: do work, signal = 0: quit
	int id; // serves as both an id and index for workers array
} thread;

typedef struct HANDLER {
	// The maximum number of workers, or rather, the number of workers to have
	int num_workers;
	// The current number of workers running (length of workers array)
	// If this is different from num_workers, need to add/remove workers
	int cur_workers;
	// A list of all threads
	thread* workers[];
} handler;

// Config filename
static char* FILE_NAME = "config.txt";

// Initialize handler struct
handler h;


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
void *work(void* ind);
// A function that is called by workers when they're cleaning up and exiting
void worker_clean(thread *t);
// A function called by the handler when it's time to clean up all workers and shutdown
void handler_clean();

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
typedef struct X {
	int i;
} x;

//------------------------------------------------------
// create_w
// * Only for use by the handler
// PURPOSE: A helper function for 'update_w', creates a single worker
//			and appends it to the end of the 'workers' array in the 'h' struct.
//------------------------------------------------------
void create_w(){

	// create thread struct
	thread *new = malloc(sizeof(thread));

	// fill in thread struct values
	new->id = h.cur_workers;
	new->signal = 1;


	printf(" * * * Worker #%d created and beginning to work.\n", new->id);
	// update handler
	h.workers[h.cur_workers] = new;
	h.cur_workers++;
	x *index = malloc(sizeof(x));
	index->i = new->id;
	pthread_create(&new->p, NULL, work, index);
	//free(index);


}

//------------------------------------------------------
// remove_w
// * Only for use by the handler
// PURPOSE: A helper function for 'update_w', removes a single worker
//			from the end of the 'workers' array in the 'h' struct.
//------------------------------------------------------
void remove_w(){

	thread *temp = h.workers[h.cur_workers - 1];
	//printf("%d = cur_workers\n",h.cur_workers);
	//printf("in remove_w w/ %d\n", temp->id);
	fflush(stdout);
	// update the signal value for the last thread
	temp->signal = 0;
	// wait for thread to rejoin
	pthread_join(temp->p, NULL); // no return values

	// remove temp from workers
	//h.workers[h.cur_workers - 1] = NULL;


}

//------------------------------------------------------
// work
//
// PURPOSE: Keeps a process 'busy', if the thread doesn't receive a
//			signal, it will be forever stuck in this function.
//------------------------------------------------------
void *work(void* ind){
	x *st = ind;
	int i = st->i;
	//printf("index: %d\n", i);
	thread *t = h.workers[i];
	//printf("%d in work \n", t->id);
	while(t->signal){
		sleep(1);
	}
	// t.signal has changed to 0, want to clean up and exit
	worker_clean(t);
	return NULL;
}

void worker_clean(thread *t){

	printf(" * * * Worker #%d cleaned up and exiting...\n", t->id);
	// update handler
	h.cur_workers--;
	free(t);
	fflush(stdout);
	// exit thread, goto join on this thread
	pthread_exit(NULL);

}

void handler_clean(){

	printf(" * Beginning total cleanup.\n");

	// iterate though list of worker threads, removing them
	while(h.cur_workers != 0){

		// remove worker
		remove_w();

		// continue loop
		//h.cur_workers--;
		// * h.cur_workers is decremented in remove_w, so no need to change loop var
	}

	printf(" * All workers have exited, handler exiting.\n");
	printf("Program completed successfully.\n");
	exit(EXIT_SUCCESS);
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
	while(1){
		sleep(1);
	}

	printf("This shouldn't have been reached...\n");
	printf("Program trying to cleanup...\n");
	handler_clean(); // maybe will work, maybe not

}
