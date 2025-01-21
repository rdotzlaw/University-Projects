#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>

#define COUNT_UP_TO 100000000

//typedef struct LOCK_TYPE{
//	atomic_int _Value;
//} lock_t;

//void mutex_lock( lock_t * );
//void mutex_unlock( lock_t * );

static pthread_mutex_t *lock;
static atomic_int counter = 0;

void *count_up( void *args )
{
    (void) args;

    for ( int i = 0; i < COUNT_UP_TO; i++ )
    {
        pthread_mutex_lock(lock);
        counter++;
		pthread_mutex_unlock(lock);
    }

    return NULL;
}

int main( void )
{
    pthread_t p1, p2;
	lock = malloc(sizeof(pthread_mutex_t));
	//lock->_Value = 0;
    pthread_create( &p1, NULL, count_up, NULL );
    pthread_create( &p2, NULL, count_up, NULL );

    pthread_join( p1, NULL );
    pthread_join( p2, NULL );
    printf( "Counter is %d\n", counter );
	free(lock);
    return EXIT_SUCCESS;
}
/*
void mutex_lock( lock_t *l )
{
    while(atomic_flag_test_and_set(l)){
		sleep(1);
	}
	l->_Value = 1;
}

void mutex_unlock( lock_t *l )
{
	l->_Value = 0;
}
*/
