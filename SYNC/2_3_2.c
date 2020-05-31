#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <time.h>
#include <unistd.h>

#define N 5 
#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 
#define LEFT (phnum + 4) % N 
#define RIGHT (phnum + 1) % N 
#define HUNGER_LIM 10.0

// philosopher states
int state[N] = {2,2,2,2,2}; 
// philosopher numbers
int phil[N] = { 0, 1, 2, 3, 4 }; 
clock_t last_ate[N];

sem_t mutex; 
sem_t S[N]; 


/*
DESCRIPTION:
	This is the same code as in 2_2_2.
	as there can be no starvation, there can also be no
	livelock, as livelock would cause a starvation.
*/

double get_time_diff_secs(clock_t time){
	return (clock() - time)/CLOCKS_PER_SEC;
}

void test(int phnum) 
{ 	
	// this time check also that neighbours
	//haven't been hungry longer than a certain time
	if (state[phnum] == HUNGRY 
		&& state[LEFT] != EATING 
		&& state[RIGHT] != EATING 
		&& (get_time_diff_secs(last_ate[LEFT]) < HUNGER_LIM)
		&& (get_time_diff_secs(last_ate[RIGHT]) < HUNGER_LIM)) { 
		// state that eating 
		state[phnum] = EATING; 


		printf("Philosopher %d takes fork %d and %d\n", 
					phnum + 1, LEFT + 1, phnum + 1); 

		printf("Philosopher %d is Eating\n", phnum + 1); 

		sleep(2); 

		last_ate[phnum] = clock();

		// sem_post(&S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(&S[phnum]); 
	} 
} 

// take up chopsticks 
void take_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that hungry 
	state[phnum] = HUNGRY; 

	printf("Philosopher %d is Hungry\n", phnum + 1); 

	// eat if neighbours are not eating 
	test(phnum); 

	sem_post(&mutex); 

	// if unable to eat wait to be signalled 
	sem_wait(&S[phnum]); 

	sleep(1); 
} 

// put down chopsticks 
void put_fork(int phnum) 
{ 

	sem_wait(&mutex); 

	// state that thinking 
	state[phnum] = THINKING; 

	printf("Philosopher %d putting fork %d and %d down\n", 
		phnum + 1, LEFT + 1, phnum + 1); 
	printf("Philosopher %d is thinking\n", phnum + 1); 

	test(RIGHT); 

	sem_post(&mutex); 
} 

void* philospher(void* num) 
{ 	

	int* i = num; 
	// initialize time last ate
	last_ate[*i] = clock();

	while (1) { 

		sleep(1); 

		take_fork(*i); 

		sleep(0); 

		put_fork(*i); 
	} 
} 

int main() 
{ 

	int i; 
	pthread_t thread_id[N]; 

	// initialize the mutexes 
	sem_init(&mutex, 0, 1); 

	for (i = 0; i < N; i++) 

		sem_init(&S[i], 0, 0); 


	for (i = 0; i < N; i++) { 

		// create philosopher processes 
		pthread_create(&thread_id[i], NULL, 
					philospher, &phil[i]); 

		printf("Philosopher %d is thinking\n", i + 1); 
	} 

 
	for (i = 0; i < N; i++) 

		pthread_join(thread_id[i], NULL); 
} 

