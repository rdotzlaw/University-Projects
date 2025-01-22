// Ryan Dotzlaw


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>
#include <time.h>

#define NANOS_PER_USEC 1000
#define USEC_PER_SEC   1000000

int cpu_count;
int slice;
char* file_name;


#define QUANT_LEN 50
#define TIME_ALLOT 200

typedef struct NODE node;
typedef struct TASK_TYPE task_t;
typedef struct MULTFEED_Q multfq;
typedef struct QUEUE Q;

typedef struct LOCK_TYPE{
	atomic_int _Value;
} lock_t;

typedef struct TASK_TYPE {
	int type;
	int len;
	int io;
	int cpu_count;
	int cpu_time;
	int allot;
	int priority;
	struct timespec *turn_end;
	struct timespec *turn_start;
	struct timespec *resp_last;
	struct timespec *resp_total;
	char *name;
}task_t;


typedef struct NODE {
	task_t *task;
	node *before;
	node *after;
} node;

typedef struct QUEUE {
	lock_t *lock;
	int size;
	node *head;
	node *tail;
} Q;


typedef struct MULTFEED_Q {
	int q_len;
	int t_all;
	Q *pri_0;
	Q *pri_1;
	Q *pri_2;
} multfq;


typedef struct CPU_TYPE{
	task_t *running;
	pthread_t *thread;
	int sig;
	int num;
} cpu_t;

cpu_t **cpu;
multfq *mlfq;

Q *made_tasks;
Q *done_tasks;

// Code from lab
void mutex_lock( lock_t *l ) {
	while(atomic_flag_test_and_set(l)){

	}
	l->_Value = 1;
}

void mutex_unlock( lock_t *l ) {
	l->_Value = 0;
}


// Given code

static void microsleep(unsigned int usecs)
{
	long seconds = usecs / USEC_PER_SEC;
	long nanos   = (usecs % USEC_PER_SEC) * NANOS_PER_USEC;
	struct timespec t = { .tv_sec = seconds, .tv_nsec = nanos };
	int ret;
	do
	{
		ret = nanosleep( &t, &t );
		// need to loop, `nanosleep` might return before sleeping
		// for the complete time (see `man nanosleep` for details)
	} while (ret == -1 && (t.tv_sec || t.tv_nsec));
}

struct timespec diff(struct timespec start, struct timespec end) {
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


// Given code ends

struct timespec tspec_add(struct timespec t1, struct timespec t2){
	struct timespec temp;
	temp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
	temp.tv_sec = t1.tv_sec + t2.tv_sec;
	return temp;
}

struct timespec tspec_div(struct timespec t, int d){
	struct timespec temp;
	//printf("tsec: %ld tnsec: %ld ", t.tv_sec, t.tv_nsec);
	long long usecs = (t.tv_sec*USEC_PER_SEC + (t.tv_nsec/NANOS_PER_USEC)) / d;
	// convert from nanos to secs & nanos
	long seconds = usecs / USEC_PER_SEC;
	long nanos   = (usecs % USEC_PER_SEC) * NANOS_PER_USEC;
	temp.tv_nsec = nanos;
	temp.tv_sec = seconds;
	//printf("psec: %ld pnsec: %ld\n", seconds, nanos);
	return temp;
}


node *init_node(task_t *t){
	node *n = malloc(sizeof(node));
	n->after = NULL;
	n->before = NULL;
	n->task = t;

	return n;
}


// Queue code


void q_lock(Q *q){
	mutex_lock(q->lock);
}
void q_unlock(Q *q){
	mutex_unlock(q->lock);
}

// removes the head node (node w/ nothing before it)
node *pop(Q *queue){
	q_lock(queue);

	if(queue->head != NULL) {
		node *ret = queue->head;
		queue->head = queue->head->after;
		if(queue->head == NULL){
			// ret was the last in the list
			queue->tail = NULL;
		} else {
			// there's a new queue->head
			queue->head->before = NULL; // remove reference from ret
		}
		ret->after = NULL;
		queue->size--;
		q_unlock(queue);
		return ret;
	} else {
		q_unlock(queue);
		return NULL;
	}
}

// adds to the tail node
void push(node *n, Q *queue){

	q_lock(queue);

	if(queue->head != NULL) {
		n->before = queue->tail;
		queue->tail->after = n;
		queue->tail = n;
	} else {
		queue->head = n;
		queue->tail = n;
	}
	queue->size++;

	q_unlock(queue);
}

int isEmpty(Q *q){
	q_lock(q);
	int ret = q->head == NULL;
	q_unlock(q);
	return ret;
}


void set_priority(node *n, int prio){

	if(prio == 0){
		push(n, mlfq->pri_0);
		n->task->priority = 0;
	} else if(prio == 1){
		push(n, mlfq->pri_1);
		n->task->priority = 1;
	} else if(prio == 2){
		push(n, mlfq->pri_2);
		n->task->priority = 2;
	}

}

#define BUF_LEN 128

void print_task(task_t *t){
	printf("%d: %s\n", t->type, t->name);
	printf(" * %d %d %d\n", t->allot, t->cpu_time, t->cpu_count);
	fflush(stdout);
}


char *d_cpy(char *buf){
	char *ret = malloc(strlen(buf));
	for (int i = 0; i < (int)strlen(buf); ++i) {
		ret[i] = buf[i];
	}
	return ret;
}

int done_reading = 0;
int first_delay = 0;
int task_c = 0;
// creates tasks by reading input from the file: 'name'
// once it reaches a delay line, delay by that much time, then continue
// once the entire 'name' file is read, set a signal to let the scheduler know
// that there are no more incoming tasks
void *set_workload(){
	FILE *f = fopen(file_name, "r");

	//printf("Set Work is : %lx\n", pthread_self());

	if(f == NULL){
		//printf("Failed to open file: '%s', exiting program.", file_name);
		exit(EXIT_FAILURE);
	}
	made_tasks = malloc(sizeof(Q));
	made_tasks->head = NULL;
	made_tasks->tail = NULL;
	char *buf = malloc(BUF_LEN);
	while(fgets(buf, BUF_LEN, f) != NULL){
		// its a delay line, do the delay, then continue
		char *buf2 = d_cpy(buf);
		if(strncmp(buf2, "DELAY", strlen("DELAY")) == 0){ // finds delay
			first_delay = 1;
			char *tok = strtok(buf, " ");
			//printf("t1: %s\n", tok);
			fflush(stdout);
			tok = strtok(NULL, " ");
			//printf("t2: %s\n", tok);
			fflush(stdout);
			//printf(" * Performing a delay for %d usec\n", atoi(tok));
			microsleep(atoi(tok));

		} else {
			// its a task of some kind
			task_c++;
			// buf: task_name task_type task_length odds_of_IO
			task_t *task = malloc(sizeof(task_t));
			char *tok = strtok(buf, " ");
			task->resp_last = NULL;
			task->resp_total = malloc(sizeof(struct timespec));
			task->turn_start = malloc(sizeof(struct timespec));
			clock_gettime(CLOCK_REALTIME, task->turn_start);
			task->turn_end = malloc(sizeof(struct timespec));
			task->name = tok;
			task->type = atoi(strtok(NULL, " "));
			task->len = atoi(strtok(NULL, " "));
			task->io = atoi(strtok(NULL, " "));
			//printf(" * Taking in %s\n", task->name);
			//print_task(task);
			node *n = init_node(task);

			// RULE 3
			set_priority(n, 0);

		}
		buf = malloc(BUF_LEN);
	}
	printf("Done reading in tasks, all tasks created and in the system.\n");
	done_reading = 1;
	return NULL;
}


int get_usec(struct timespec t){
	int sec_usec = t.tv_sec * USEC_PER_SEC;
	int nan_usec = t.tv_nsec / NANOS_PER_USEC;
	return sec_usec + nan_usec;
}

/*
int avg_turn;
int avg_resp;
*/
int done = 0;
int task_done = 0;
void compile_data(){

	while(done != cpu_count){

	}


	int total_cpu[4] = {0, 0, 0, 0};
	struct timespec spec_turn[4];
	spec_turn[0].tv_nsec = 0;
	spec_turn[0].tv_sec = 0;
	spec_turn[1].tv_nsec = 0;
	spec_turn[1].tv_sec = 0;
	spec_turn[2].tv_nsec = 0;
	spec_turn[2].tv_sec = 0;
	spec_turn[3].tv_nsec = 0;
	spec_turn[3].tv_sec = 0;
	//int total_resp[4];
	int type_count[4] = {0, 0, 0, 0};
	int total_cpu_time = 0;
	struct timespec resp[4];
	//long long total_turn[4] = {0, 0, 0, 0};
	resp[0].tv_nsec = 0;
	resp[0].tv_sec = 0;
	resp[1].tv_nsec = 0;
	resp[1].tv_sec = 0;
	resp[2].tv_nsec = 0;
	resp[2].tv_sec = 0;
	resp[3].tv_nsec = 0;
	resp[3].tv_sec = 0;
	//printf("Total Tasks: %d\n", done_tasks->size);
	//printf("Total Tasks: %d\n", task_c);
	while(done_tasks->head != NULL){
		task_t *t = pop(done_tasks)->task;
		int type = t->type;
		type_count[type]++;
		spec_turn[type] = tspec_add(spec_turn[type], *t->turn_end);
		resp[type] = tspec_add(resp[type], *t->resp_last);

		total_cpu[type] += t->cpu_count;
		total_cpu_time += t->cpu_time;
	}

	resp[0] = tspec_div(resp[0], type_count[0]);
	resp[1] = tspec_div(resp[1], type_count[1]);
	resp[2] = tspec_div(resp[2], type_count[2]);
	resp[3] = tspec_div(resp[3], type_count[3]);

	//printf("0\n");
	spec_turn[0] = tspec_div(spec_turn[0], type_count[0]);
	//printf("1\n");
	spec_turn[1] = tspec_div(spec_turn[1], type_count[1]);
	//printf("2\n");
	spec_turn[2] = tspec_div(spec_turn[2], type_count[2]);
	//printf("3\n");
	spec_turn[3] = tspec_div(spec_turn[3], type_count[3]);


	printf("\n----------Program Complete----------\nUsing mlfq with %d CPUs\n", cpu_count);
	printf("Average turnaround time per type:\n"); // turnaround time is the time taken from the entry in the system till the exit
	printf(" * Type %d: %d usec\n", 0, get_usec(spec_turn[0]));
	printf(" * Type %d: %d usec\n", 1, get_usec(spec_turn[1]));
	printf(" * Type %d: %d usec\n", 2, get_usec(spec_turn[2]));
	printf(" * Type %d: %d usec\n", 3, get_usec(spec_turn[3]));
	printf(" * * Turnaround time is the time it takes for a task to exit the system after entering.\n");

	printf("\nAverage response time per type:\n"); // response time is the time taken between entering the system and the first time on the cpu
	printf(" * Type %d: %d usec\n", 0, get_usec(resp[0]));
	printf(" * Type %d: %d usec\n", 1, get_usec(resp[1]));
	printf(" * Type %d: %d usec\n", 2, get_usec(resp[2]));
	printf(" * Type %d: %d usec\n", 3, get_usec(resp[3]));
	printf(" * * Response time is the time taken between a task's arrival in the system, and it's first turn on the CPU\n");

	printf("\nTotal Context Switches: %d\n", total_cpu[0]+total_cpu[1]+total_cpu[2]+total_cpu[3]);
	printf("Total time spent by all tasks: %Lf sec\n", (long double)(total_cpu_time)/USEC_PER_SEC);
	printf("Total Tasks: %d\n", type_count[0]+type_count[1]+type_count[2]+type_count[3]);
	printf(" * Type %d tasks: %d\n", 0, type_count[0]);
	printf(" * Type %d tasks: %d\n", 1, type_count[1]);
	printf(" * Type %d tasks: %d\n", 2, type_count[2]);
	printf(" * Type %d tasks: %d\n", 3, type_count[3]);

}



int c_cpu = 0;
lock_t *cpu_lock;
// does the 'execution' of the task given
// then either sends task to the done queue
// or reschedules at the appropriate priority
void *CPU(){
	mutex_lock(cpu_lock);
	int index = c_cpu;
	c_cpu++;
	mutex_unlock(cpu_lock);
	printf(" * CPU %d ONLINE :)\n", index);
	//fflush(stdout);

	cpu_t *this = cpu[index];
	while(1) {

		task_t *t = this->running;

		// since the task is being pop'd off the queue and passed to the CPU
		// if the running task is NULL, there's nothing to do

		if(t != NULL){

			// the time execution for the current task starts
			struct timespec *start = malloc(sizeof(struct timespec));
			clock_gettime(CLOCK_REALTIME, start);

			//printf(" %d: %s", index, t->name);

			if(t->resp_last == NULL){
				// first time executing
				// response time = the time between being created and starting execution
				t->resp_last = malloc(sizeof(struct timespec));
				*t->resp_last = diff(*t->turn_start, *start);
			}

			//print_task(t);

			//srand(1);
			int io_chance = (rand() % (100 - 0 + 1));
			t->cpu_count++;
			if (io_chance <= t->io) {
				// doing I/O
				int io_time = (rand() % (QUANT_LEN - 0 + 1));
				microsleep(io_time);
				//printf("IO for %s of length: %d, has %d exec time on CPU\n", t->name, io_time, t->len);
				t->cpu_time += io_time;
				t->allot += io_time;
			} else {
				// doing normal execution

				// run for complete time-slice
				// or run to completion, then task is done
				int time = t->len - QUANT_LEN;
				if (time <= 0) {
					// task is done!
					if (time < 0) {
						microsleep(QUANT_LEN + time);
						t->cpu_time += (QUANT_LEN + time);
					} else {
						microsleep(QUANT_LEN);
						t->cpu_time += QUANT_LEN;
					}
					t->len = 0;
					// put in done list
					struct timespec *task_comp = malloc(sizeof(struct timespec));
					clock_gettime(CLOCK_REALTIME, task_comp);
					*t->turn_end = diff(*t->turn_start, *task_comp);
					node *n_done = init_node(t);
					//printf(" * * CPU %d completed: %s! In %ld usecs!\n", index, t->name, t->turn_end->tv_nsec/NANOS_PER_USEC + t->turn_end->tv_sec*USEC_PER_SEC);
					task_done++;
					push(n_done, done_tasks);

				} else {
					// task isn't done, needs reschedule
					microsleep(QUANT_LEN);
					t->cpu_time += QUANT_LEN;
					t->allot += QUANT_LEN;
					t->len = time;

					// RESCHEDULE HERE
					node *n = init_node(t);
					// RULE 4
					if (TIME_ALLOT % t->allot >= 4) {
						// used up all of time slice
						// lower priority
						//printf("\n%s used it's time slice\n", t->name);
						int pri;
						if (t->priority == 2) {
							// lowest priority already
							pri = 2;
						} else {
							pri = t->priority + 1;
						}
						//printf(" * * Rescheduling %s from %d to priority: %d\n", t->name, t->priority, pri);
						set_priority(n, pri);
					} else {
						// reschedule at same priority
						//printf(" * * Rescheduling %s at priority: %d\n", t->name, t->priority);
						set_priority(n, t->priority);
					}


				}
				this->running = NULL;
			}
		} else if(t == NULL && this->sig == -1 && isEmpty(mlfq->pri_0) && isEmpty(mlfq->pri_1) && isEmpty(mlfq->pri_2)){
			printf("CPU %d is done!\n", index);
			done++;
			return NULL;
		} else if(this->sig == -1){ // debug
			//printf("%d %d %d %d %d\n",t == NULL, this->sig == -1, isEmpty(mlfq->pri_0), isEmpty(mlfq->pri_1), isEmpty(mlfq->pri_2));
		}

	}

}

int debug = 0;
struct timespec *last_S = NULL;
// The Scheduler is what gives out the tasks to the CPUs
void *schedule(){
	//last_S = malloc(sizeof(struct timespec));
	// wait until first delay
	while(!first_delay){

	}
	//printf("First delay over, begin scheduling\n");

	// mlfq now should have some stuff in it


	// main schedule loop
	while(1) {
		if(debug)
			printf("\rpri_0: %d pri_1: %d pri_2: %d", mlfq->pri_0->size, mlfq->pri_1->size, mlfq->pri_2->size);

		struct timespec *sched_cycle = malloc(sizeof(struct timespec));
		clock_gettime(CLOCK_REALTIME, sched_cycle);

		// RULE 5 is here
		struct timespec temp;
		if(last_S == NULL) {
			// first sched cycle
			last_S = sched_cycle;
		} else if((temp = diff(*last_S, *sched_cycle)), (temp.tv_nsec/NANOS_PER_USEC + temp.tv_sec*USEC_PER_SEC) - slice >= 0){
			// the difference is positive/zero -> it has been 'slice' usec since last_S
			// RULE 5
			// shift priority of EVERY task in mlfq to highest
			//node *n = NULL;
			//printf("Boost priority, gap : %ld\n", diff(*last_S, *sched_cycle).tv_nsec/NANOS_PER_USEC);


			while(1){
				node *n = NULL;
				if (!isEmpty(mlfq->pri_1)) {
					n = pop(mlfq->pri_1);
					set_priority(n, 0);
				} else if (!isEmpty(mlfq->pri_2)) {
					n = pop(mlfq->pri_2);
					set_priority(n, 0);
				} else {
					break;
				}
			}


			/* This is somehow slower than the while loop above...
			// manually move head over to prio_0 queue
			if (!isEmpty(mlfq->pri_1)) {
				//printf("Locking pri_1\n");
				q_lock(mlfq->pri_1);
				node *n = mlfq->pri_1->head;


				if(!isEmpty(mlfq->pri_0)) {
					//printf("Locking pri_0\n");
					q_lock(mlfq->pri_0);
					//printf("Swapping\n");
					mlfq->pri_0->tail->after = n;
					n->before = mlfq->pri_0->tail;
					mlfq->pri_0->tail = mlfq->pri_1->tail;
					mlfq->pri_0->size += mlfq->pri_1->size;
					//printf("Unlocking pri_0\n");
					q_unlock(mlfq->pri_0);
				} else {
					//printf("Locking pri_0\n");
					q_lock(mlfq->pri_0);

					mlfq->pri_0->head = mlfq->pri_1->head;
					mlfq->pri_0->tail = mlfq->pri_1->tail;
					mlfq->pri_0->size = mlfq->pri_1->size;

					//printf("Unlocking pri_0\n");
					q_unlock(mlfq->pri_0);
				}

				//printf("Nulling\n");
				mlfq->pri_1->head = NULL;
				mlfq->pri_1->tail = NULL;
				//printf("Unlocking pri_0\n");
				q_unlock(mlfq->pri_1);

			}
			// manually move head over to prio_0 queue
			if (!isEmpty(mlfq->pri_2)) {
				//printf("Locking pri_2\n");
				q_lock(mlfq->pri_2);
				node *n = mlfq->pri_2->head;

				if(!isEmpty(mlfq->pri_0)) {
					//printf("Locking pri_0\n");
					q_lock(mlfq->pri_0);
					//printf("Swapping\n");
					mlfq->pri_0->tail->after = n;
					n->before = mlfq->pri_0->tail;
					mlfq->pri_0->tail = mlfq->pri_2->tail;
					mlfq->pri_0->size += mlfq->pri_2->size;
					//printf("Unlocking pri_0\n");
					q_unlock(mlfq->pri_0);
				}else {
					//printf("Locking pri_0\n");
					q_lock(mlfq->pri_0);

					mlfq->pri_0->head = mlfq->pri_2->head;
					mlfq->pri_0->tail = mlfq->pri_2->tail;
					mlfq->pri_0->size = mlfq->pri_2->size;

					//printf("Unlocking pri_0\n");
					q_unlock(mlfq->pri_0);
				}

				//printf("Nulling\n");
				mlfq->pri_2->head = NULL;
				mlfq->pri_2->tail = NULL;
				//printf("Unlocking pri_2\n");
				q_unlock(mlfq->pri_2);
			}
			*/
			// set last_S to sched_cycle
			last_S = sched_cycle;

		} // continue as normal


		task_t *t = NULL;
		node *n = NULL;
		// get the highest priority task possible
		// the nature of the push/pop operations means
		// if a task is rescheduled, it's rescheduled at the end
		// RULE 1 & 2
		if (!isEmpty(mlfq->pri_0)) {
			n = pop(mlfq->pri_0);
			t = n->task;
		} else if (!isEmpty(mlfq->pri_1)) {
			n = pop(mlfq->pri_1);
			t = n->task;
		} else if (!isEmpty(mlfq->pri_2)) {
			n = pop(mlfq->pri_2);
			t = n->task;
		} else if (!done_reading) {
			// wait
			//printf("Not done reading!!!");
			//fflush(stdout);
		} else if(done_reading && task_done == task_c){
			//printf("Done reading, all queues empty, signalling CPU's\n");
			//fflush(stdout);
			// done, tell CPU's to end
			for (int i = 0; i < cpu_count; ++i) {
				cpu[i]->sig = -1;
			}

			//compile_data();
			return NULL;
		} else {
			//printf("%d %d %d %d \n",done_reading, task_done == task_c, task_done, task_c);
			//sleep(1);
		}

		// SCHEDULE TASK IN CPU
		// now have task to schedule
		// give to the earliest CPU w/ no task
		// loop until task is given
		if(t != NULL) { // not waiting for more tasks
			//printf(" * Scheduling task: %s\n", t->name);
			int i = 0;
			while (1) {
				if (cpu[i]->running == NULL) {
					// give task
					cpu[i]->running = t;
					//printf(" * Scheduled: %s for CPU %d\n", t->name, i);
					break;
				}
				// prevent int overflow (impossible since a task will eventually complete, but still...)
				i = (i + 1) % cpu_count;

			}
		}
	}


}

multfq *init_mlfq(){
	Q *q1 = malloc(sizeof(Q));
	q1->size = 0;
	q1->lock = malloc(sizeof(lock_t));
	Q *q2 = malloc(sizeof(Q));
	q2->size = 0;
	q2->lock = malloc(sizeof(lock_t));
	Q *q3 = malloc(sizeof(Q));
	q3->size = 0;
	q3->lock = malloc(sizeof(lock_t));

	multfq *ret = NULL;
	ret = malloc(sizeof(multfq));
	ret->pri_0 = q1;
	ret->pri_1 = q2;
	ret->pri_2 = q3;

	ret->q_len = QUANT_LEN;
	ret->t_all = TIME_ALLOT;
	return ret;

}



void init_data(){
	cpu = malloc(sizeof(cpu_t *) * cpu_count);
	// create cpu structs
	for (int i = 0; i < cpu_count; ++i) {
		cpu[i] = malloc(sizeof(cpu_t));
		cpu[i]->num = i;
		cpu[i]->running = NULL;
		cpu[i]->thread = malloc(sizeof(pthread_t));
	}
	mlfq = init_mlfq();
	done_tasks = malloc(sizeof(Q));
	done_tasks->size = 0;
	done_tasks->lock = malloc(sizeof(lock_t));

}


int main(int argc, char *argv[]) {
	(void) argc; // don't care...

	// create threads
	// thread for reading
	// thread for scheduler
	// 'n' threads for CPU

	cpu_count = atoi(argv[1]);
	slice = atoi(argv[2]);
	file_name = argv[3];

	debug = argc == 5;

	pthread_t t_read, t_sched;

	init_data();

	// get the tasks
	pthread_create(&t_read, NULL, set_workload, NULL);

	// start the scheduler
	pthread_create(&t_sched, NULL, schedule, NULL);

	// start cpus
	//int **ids= malloc(sizeof(int) * cpu_count);
	cpu_lock = malloc(sizeof(lock_t));
	for (int i = 0; i < cpu_count; ++i) {
		//printf("Creating CPU: %d\n", i);
		//*ids[i] = i;
		pthread_create(cpu[i]->thread, NULL, CPU, NULL);//(void *)ids[i]);
	}

	// join threads
	pthread_join(t_read, NULL);

	pthread_join(t_sched, NULL);

	for (int i = 0; i < cpu_count; ++i) {
		pthread_join(*cpu[i]->thread, NULL);
	}


	compile_data();

	exit(EXIT_SUCCESS);


}
