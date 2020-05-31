#include <pthread.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <unistd.h>

#define N 5 

#define THINKING 2 
#define HUNGRY 1 
#define EATING 0 

#define FREE -1

#define LEFT (phnum + 4) % N 
#define RIGHT (phnum + 1) % N 

// philosopher states
int state[N]; 
// philosopher numbers
int phil[N] = { 0, 1, 2, 3, 4 }; 
// fork numbers,fork 0 is 
// to the left of philosopher 0
int forks[N] = {-1, -1,-1,-1,-1};

sem_t mutex; 
sem_t S[N]; 

/*

DESCRIPTION:

	Instead of checking whether the philosophers neighbors are eating,
	he now checks whether one of the forks is freem if it is he takes it.
	When he takes both forks he starts eating.
	The deadlock happens when all philosophers have taken the fork on their
	left (or on their right, what matters is that it's the same one for all)
	and therefore they will always wait for the other fork to become free
	which will never happen.
*/



void test_lfork(int phnum) 
{ 
	if (state[phnum] == HUNGRY 
		&& forks[phnum] == FREE) { 
		forks[phnum] = phnum;

		printf("Philosopher %d takes fork %d (left fork)\n", 
					phnum + 1, phnum +1); 

		if(forks[phnum] == phnum && forks[phnum+1] == phnum)
			printf("Philosopher %d is Eating\n", phnum + 1); 

		sleep(1); 

		// sem_post(&S[phnum]) has no effect 
		// during takefork 
		// used to wake up hungry philosophers 
		// during putfork 
		sem_post(&S[phnum]); 
	} 
} 
void test_rfork(int phnum) 
{ 
	if (state[phnum] == HUNGRY 
		&& forks[phnum+1] == FREE) { 
		forks[phnum+1] = phnum;

		printf("Philosopher %d takes fork %d (right fork)\n", 
					phnum + 1, phnum +2); 

		if(forks[phnum] == phnum && forks[phnum+1] == phnum)
			printf("Philosopher %d is Eating\n", phnum + 1); 

		sleep(1); 


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
	test_lfork(phnum); 
	test_rfork(phnum);

	sem_post(&mutex); 

	// if unable to eat wait to be signalled 
	sem_wait(&S[phnum]); sem_wait(&S[phnum]); 

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
	
	forks[phnum] = FREE;
	forks[phnum+1] = FREE;

	printf("Philosopher %d is thinking\n", phnum + 1); 
	test_rfork(LEFT); 
	test_lfork(RIGHT); 

	sem_post(&mutex); 
} 

void* philospher(void* num) 
{ 

	while (1) { 

		int* i = num; 

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

