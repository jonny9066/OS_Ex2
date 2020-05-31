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

#define SUCCESS 1
#define FAIL 0

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
	The philosophers 1 and 3 start first, the others
	start 2 seconds later. A philosopher thinks 1 second
	and eats for 3 seconds. When a philosopher can't take
	both forks he waits 4 seconds. When the three philosophers
	join the the first two are in the middle of eating,
	so the ones who joined last wait 4 seconds. By this time the first two
	have finished eating, have finished thinking and are
	eating again and the three who joined last catch them
	at the same time of eating as when they joined, this repeats ad infinitum,
	thus the three philosophers starve.
	
*/


// changes state, and prints out and sleeps accodringly
void change_state(int phnum, int s){

	sem_wait(&mutex);
	state[phnum] = s; 
	sem_post(&mutex);

	if(s == THINKING){
		printf("Philosopher %d is Thinking\n", phnum + 1);
	}
	else if(s == HUNGRY){
		printf("Philosopher %d is Hungry\n", phnum + 1); 
	}
	else if(s == EATING){
		printf("Philosopher %d is Eating\n", phnum + 1); 
	}
	
}


// take up both forks if thay are free
int take_forks(int phnum) 
{ 
	int r = FAIL;
	sem_wait(&mutex); 

	if (forks[phnum] == FREE
	&& forks[phnum+1] == FREE) { 

		forks[phnum] = phnum;

		printf("Philosopher %d takes forks %d and %d\n", 
					phnum + 1, phnum +1, phnum +2); 
		r = SUCCESS;
	} 
	sem_post(&mutex); 

	return r;
} 


// put down both forks
void put_forks(int phnum) 
{ 
	sem_wait(&mutex); 

	printf("Philosopher %d putting forks %d and %d down\n", 
		phnum + 1, phnum + 1, phnum + 2); 
	
	forks[phnum] = FREE;
	forks[phnum+1] = FREE;

	sem_post(&mutex); 
} 

void* philospher(void* num) 
{ 	
	int* i = num; 
	change_state(*i, THINKING);
	sleep(1);
	change_state(*i, HUNGRY);

	while (1) { 

		

		// try to take forks, if success eat
		// else wait
		if(take_forks(*i) == SUCCESS){
			change_state(*i, EATING);
			sleep(3);
			put_forks(*i);
			change_state(*i, THINKING);
			sleep(1);
			change_state(*i, HUNGRY);
		}
        else{
			sleep(4);
        }

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

	// create first 2 philosophers
	pthread_create(&thread_id[2], NULL, 
					philospher, &phil[2]); 
	pthread_create(&thread_id[4], NULL, 
					philospher, &phil[4]); 

	// create the rest after 2 second wait
	sleep(2);
	pthread_create(&thread_id[0], NULL, 
					philospher, &phil[0]); 				
	pthread_create(&thread_id[1], NULL, 
					philospher, &phil[1]); 	
	pthread_create(&thread_id[4], NULL, 
					philospher, &phil[4]); 

	for (i = 0; i < N; i++) 

		pthread_join(thread_id[i], NULL); 
} 

