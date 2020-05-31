#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define MAX_SCORE 5
//#define DEBUG

pid_t pid;
int score = 0;
int size;
int fdr;
int fdw;
char * fifo1 = "/tmp/fifo1";
char * fifo2 = "/tmp/fifo2";

void pingpongSignalHandler (int signum){
    #ifdef DEBUG
    printf("DEBUG:entered signal handler\n");
    #endif
    
    assert((fdr = open(fifo1, O_RDONLY)) >=0);
    #ifdef DEBUG
    printf("DEBUG:opened fifo for read in signal handler\n");
    #endif
    if((size = read(fdr, &score, sizeof(score))) > 0){
        close(fdr);
        printf("%d\n", score);
        if(score < MAX_SCORE){
            usleep(1000000); // sleep 1 sec
            // signal other procces to start listening
            kill(pid, SIGUSR1);
            // increment and send score
            score++;
            fdw = open(fifo2, O_WRONLY);
            write(fdw, &score, sizeof(score));
            close(fdw);
            #ifdef DEBUG
            printf("DEBUG:incremented and returned score\n");
            #endif
        }
        else{
            //finish game and clean up
            printf("Game Over\n");
            remove("/tmp/fifo1");
            remove("/tmp/fifo2");
        }
    }
    else{
        printf("An error occured while reading form the buffer\n");
        exit(-1);
    }

}


int main(){
    
    #ifdef DEBUG
    printf("*** DEBUG START***\n");
    #endif

    signal(SIGUSR1, pingpongSignalHandler);
    #ifdef DEBUG
    printf("DEBUG:creating fifos\n");
    #endif
    
    // create fifos
    assert(mkfifo(fifo1, 0666) == 0);
    assert(mkfifo(fifo2, 0666) == 0);
    #ifdef DEBUG
    printf("DEBUG:creating child\n");
    #endif
    pid = fork();
    // check if child
    if(pid == 0){  
        // set signal handler
        signal(SIGUSR1, pingpongSignalHandler);
        // get parent pid
        pid = getppid();
        #ifdef DEBUG
        printf("DEBUG:inside child, parent pid is %d\n", pid);
        #endif

        //swap fifos so that what is read for child is write for parent
        char* tmp = fifo1;
        fifo1=fifo2;
        fifo2=tmp;

        //signal parent to start listening to message
        kill(pid, SIGUSR1);

        //write first message. note that fifo2 is for writing in
        // the signal function too.
        fdw = open(fifo2, O_WRONLY);
        write(fdw, &score, sizeof(score));
        close(fdw);
        #ifdef DEBUG
        printf("DEBUG:wrote score and closed in child\n");
        #endif
        while(score<MAX_SCORE){
                #ifdef DEBUG
                printf("DEBUG:child still alive\n");
                usleep(3000000);
                #endif
        }
        usleep(1000000);
        printf("Child is going to be terminated\n");
    }
    //else parent
    else{
        
        #ifdef DEBUG
        printf("DEBUG:inside parent, child pid is %d\n", pid);
        #endif

        while(score<MAX_SCORE){
                #ifdef DEBUG
                printf("DEBUG:parent still alive\n");
                usleep(3000000);
                #endif
        }
        usleep(1000000);
        printf("Parent is going to be terminated\n");
    }

}