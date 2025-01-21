#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <ctype.h>

#include <string.h>

 typedef struct DATA_TYPE{
	pthread_mutex_t *lock;
	int sig;
	char *buf;
} data_t;

int up_loop, lo_loop;

data_t *main_dat;
data_t *lower_dat;
//data_t *upper_dat;


void set_buf(data_t *dat, char *b){

	pthread_mutex_lock(dat->lock);
	dat->buf = b;
	pthread_mutex_unlock(dat->lock);
}

char* get_buf(data_t *dat){
	return dat->buf;
}

void *to_lower(){
	while(lo_loop) {
		if(main_dat->sig) {
			printf("to lower\n");
			fflush(stdout);
			// allocate new string
			pthread_mutex_lock(main_dat->lock);
			char *old_buf = get_buf(main_dat);
			pthread_mutex_unlock(main_dat->lock);

			char *new_buf = malloc(sizeof(old_buf));

			for (unsigned long i = 0; i < strlen(old_buf); i++) {
				new_buf[i] = (char) tolower((unsigned char) old_buf[i]);
			}
			printf("-> %s", new_buf);
			set_buf(lower_dat, new_buf);
			main_dat->sig = 0;
			lower_dat->sig = 1;
		}
	}
	return NULL;
}

void *to_upper(){
	while(up_loop) {
		if(lower_dat->sig) {
			printf("to upper\n");
			fflush(stdout);
			pthread_mutex_lock(main_dat->lock);
			char *old_buf = get_buf(lower_dat);
			pthread_mutex_unlock(main_dat->lock);

			char *new_buf = malloc(sizeof(old_buf));


			for (unsigned long i = 0; i < strlen(old_buf); i++) {
				new_buf[i] = (char) toupper((unsigned char) old_buf[i]);
			}
			printf("-> %s", new_buf);
			lower_dat->sig = 0;
		}
	}
	return NULL;
}


int main() {
	char *buf = malloc(128); // 128 char max string

	// create threads
	pthread_t lower, upper;



	up_loop = 1;
	lo_loop = 1;

	//main_dat->buf = malloc(128);
	main_dat = malloc(sizeof(data_t));

	//lower_dat->buf = malloc(128);
	lower_dat = malloc(sizeof(data_t));

	pthread_mutex_init(main_dat->lock, NULL);
	pthread_mutex_init(lower_dat->lock, NULL);

	main_dat->sig = 0;
	lower_dat->sig = 0;

	//fgets(buf, 128, stdin);
	//set_buf(main_dat, buf);
	pthread_create(&lower, NULL, to_lower, NULL );
	pthread_create(&upper, NULL, to_upper, NULL );
	while(buf[0] != '\04') { // loop forever (until interrupt)
		// pass buf to lower thread
		// (do safely w/ locks)

		fgets(buf, 128, stdin);
		printf("-> %s", buf);
		fflush(stdout);
		set_buf(main_dat, buf);
		main_dat->sig = 1;

	}
	up_loop = 0;
	lo_loop = 0;

	pthread_join(lower, NULL);
	pthread_join(upper, NULL);





}
