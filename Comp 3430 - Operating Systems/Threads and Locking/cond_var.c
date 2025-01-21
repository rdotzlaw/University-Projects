
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <ctype.h>
#include <string.h>


/*/ main
 / loops and grabs input from usr
 /
 /
 /
 /
 /
 /
 /
/*/


typedef struct LOCK_TYPE{
	atomic_int _Value;
} lock_t;


typedef struct DATA_TYPE{
	lock_t *lock;
	int sig;
	char *buf;
} data_t;

int up_loop, lo_loop;
data_t * main_dat;
data_t * lower_dat;

void mutex_lock( lock_t * );
void mutex_unlock( lock_t * );


char *strlwr(char *buf){
	unsigned char *new = (unsigned char *)buf;

	while(*new) {
		*new = tolower((unsigned char)*new);
		new++;
	}

	return buf;
}

char *strupr(char *buf){
	//printf("buffing up\n");
	unsigned char *new = (unsigned char *)buf;

	while(*new) {
		//printf("infinity!!");
		fflush(stdout);

		*new = toupper((unsigned char)*new);
		new++;
	}

	return buf;
}

void set_buf(data_t *dat, char *buf){
	mutex_lock(dat->lock);
	dat->buf = buf;
	//printf("buf: %s\n", buf);
	dat->sig = 1;
	mutex_unlock(dat->lock);
}

// deep copy
char *get_buf(data_t *dat){
	mutex_lock(dat->lock);
	char *new = malloc(sizeof(dat->buf));
	for(int i = 0; i < (int)strlen(dat->buf); i++){
		new[i] = dat->buf[i];
	}
	mutex_unlock(dat->lock);
	return new;
}

void *to_lower(){

	while(lo_loop){
		if(main_dat->sig){
			// ready for processing
			// get buffer, use locks
			char *buf = get_buf(main_dat);

			// convert to lowercase

			buf = strlwr(buf);


			// print out
			printf("--> %s", buf);
			fflush(stdout);


			// pass into lower_dat

			set_buf(lower_dat, buf);

			main_dat->sig = 0;
		} else {
			usleep(50000);
		}
	}

	return NULL;

}

void *to_upper(){

	while(up_loop){
		if(lower_dat->sig){
			//printf("getting buf\n");
			// ready for processing
			// get buffer, use locks
			char *buf = get_buf(lower_dat);

			//printf("buf gotten\n");
			// convert to upper

			buf = strupr(buf);

			//printf("buffed up\n");
			// print out
			printf("--> %s", buf);
			fflush(stdout);


			free(buf);
			lower_dat->sig = 0;
		} else {
			usleep(50000);
		}
	}

	return NULL;

}


int main() {
	char *buf = malloc(128); // 128 char max string

	// create threads
	pthread_t lower, upper;

	printf("Starting program.\n * Press [ENTER] alone to quit.\n");

	up_loop = 1;
	lo_loop = 1;

	//main_dat->buf = malloc(128);
	main_dat = malloc(sizeof(data_t));
	main_dat->lock = malloc(sizeof(lock_t));

	//lower_dat->buf = malloc(128);
	lower_dat = malloc(sizeof(data_t));
	lower_dat->lock = malloc(sizeof(lock_t));

	main_dat->sig = 0;
	lower_dat->sig = 0;

	pthread_create(&lower, NULL, to_lower, NULL );
	pthread_create(&upper, NULL, to_upper, NULL );

	fgets(buf, 128, stdin);
	while(buf[0] != '\n') { // loop forever (until interrupt)
		// pass buf to lower thread
		// (do safely w/ locks)
		printf("-> %s", buf);
		fflush(stdout);
		set_buf(main_dat, buf);
		fgets(buf, 128, stdin);

	}
	up_loop = 0;
	lo_loop = 0;

	// join threads
	pthread_join(lower, NULL);
	pthread_join(upper, NULL);

	printf("Program execution completed.\n");

}

void mutex_lock( lock_t *l ) {
	while(atomic_flag_test_and_set(l)){
		sleep(1);
	}
	l->_Value = 1;
}

void mutex_unlock( lock_t *l ) {
	l->_Value = 0;
}

