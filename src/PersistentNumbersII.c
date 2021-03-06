/*
* Author: Beesham Sarendranauth | 104854956
* Date: 2019/01/24
* Class: Operating Systems Fundamentals (COMP3300)
* Title: PersistentNumbersII.c (Homework #1) 
* Description: This program takes a file with a list of numbers
* The persistency of each number will be calculated and the max and min persitent will 
* be printed. The program will also calulate the execution time. 
* This version separates the work via multiple processes (fork) 
* Assumptions: 
* - a number is 9 digits long and is line separated
* - the amount of lines in the file is known
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include<sys/wait.h>

#define SIZE_OF_ARRAY 125
#define MAX_CHILD_PROCESSES 5
#define WORK_PORTION_PER_CHILD 25

// holds the start and end positions for portion of work to be completed
struct WorkPortion {
    int start;
    int end;    
} workPortion;

bool argumentCheck(); //check if any args were provided
void getPersistent(int *array); //calculates the persistency of the number
bool readFile(int *array, char *file[]); //reads the file for list of numbers
void sort(int *arr, int a, int b); //sorts the input 
int spawnChild(); //spawns a child process 
void signalHandlerChild(int signo); //Signal Handler for child processes
void doChildWork(); //child process calculates persistence of numbers
void createPipe(); //creates the pipe needed to communicate with the child processes
void createChildProcesses(int *pid, int *childCount); //creates multiple child processes
void printParentMessage();
void assignWorkPortion(); //write a portion of work to the pipe
void unpauseChildProcesses();
void waitForChildProcesses();

int childPids[MAX_CHILD_PROCESSES]; //List of child pids the parent keeps as a queue
int FLAG_CONT = 0; //flag to notify weather or not to keep sending kills to unpause child
int numbers[SIZE_OF_ARRAY]; //array to hold the list of numbers
int fd[2]; //pipe file descriptors

int main(int argc, char *argv[]) {  
    int childCount = 0;
    int pid;    
    double executionTime = 0;
    clock_t startTime = clock();
    
    //checks for correct arg count and reads the supplied file if exists
    if (!argumentCheck(argc) || !readFile(numbers, argv)) {
        clock(); // stops the clock if error
        exit(0);
    }
    
    sort(numbers, 0, SIZE_OF_ARRAY - 1);
    createPipe();
    createChildProcesses(&pid, &childCount);

    //do parent stuff
    if (pid > 0) {    
        close(fd[0]); //close read 

        printParentMessage();
        assignWorkPortion();
        unpauseChildProcesses();
        waitForChildProcesses();      

        clock_t endTime = clock();
        executionTime += (double)(endTime - startTime) / CLOCKS_PER_SEC;

        printf("Execution time: %f seconds\nGood Bye!\n", executionTime);
    }

    //do child stuff
    if (pid == 0) {
        signal(SIGUSR1, signalHandlerChild);
        pause();
        
        printf("I am kid #: %d and my id is: %d\n", childCount, getpid());
        
        close(fd[1]); //close write side of pipe
        
        struct WorkPortion wp;
        read(fd[0], &wp, sizeof(struct WorkPortion)); //read start/end position
        doChildWork(wp);
        
        exit(1);
    }
}

void waitForChildProcesses() {
    int child_pid;
    int status;
    int n = MAX_CHILD_PROCESSES;
    //waits for the children to finish
    while(n > 0) {
        child_pid = wait(&status);
        printf("From main: child, id = %d ended with status %d\n", child_pid, status);
        --n;
    }
}

void unpauseChildProcesses() {
    //Un-pauses the child precesses      
    for(int i = 0;i < 5; i++) {
        kill(childPids[i], SIGUSR1);
    }
}

void assignWorkPortion() {
    int portions[5][2] = {{0,24},{25,49},{50,74},{75,99},{100,124}}; //1/5th of file being processes. i.e MAX_ARRAY_SIZE
    //write a portion of work to the pipe to be read by a process
    //whichever process reads it first
    for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
        struct WorkPortion a;
        a.start = *portions[i];
        a.end = portions[i][1];
        write(fd[1], &a, sizeof(struct WorkPortion));
    }
}

void printParentMessage() {
    printf("I am the father of the following: "); 
    for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
        if(i == MAX_CHILD_PROCESSES - 1) {
            printf("and %d", childPids[i]);
        } else printf("%d, ", childPids[i]);
            
    }
    printf("\n");
}

void createChildProcesses(int *pid, int *childCount) {
    for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
        if((*pid = spawnChild(childCount)) != 0) {
            childPids[i] = *pid;
        } else break;
    }
}

void createPipe() {
    if(pipe(fd) == -1) {
        printf("Error: Failed to create pipe");
        exit(1);
    }
}

void doChildWork(struct WorkPortion wp) {
    int temp[WORK_PORTION_PER_CHILD];
    
    //creates a temp array holding the portion of the list to compute
    for(int i=wp.start, j=0;j<WORK_PORTION_PER_CHILD; i++,j++) {
        temp[j] = numbers[i];
    }

    getPersistent(temp);    
}

int spawnChild(int *childCount) {
    int pid;
    if ((pid = fork()) != 0) {
        (*childCount)++;
        return pid;
    }
    return 0;
}

/*
* Calculates the persistence of an integer number
*/
int calculatePersistent(int a) {
    int newNum = 1;
    int persistency = 0;
    int number = a;    

    //calculates the persistency of the number
    while (number % 10 != number) { //exit loop if single digit, e.g 8%10=8
        int tempNum = number;
        int factor = 1;
        int digit = 0;
        
        //finds the significance of the number, e.g 10, 100, 1000
        while (tempNum) {
            tempNum = tempNum/10;
            factor = factor*10;
        }
        
        tempNum = number;
        
        //finds the digit by significance and multiplies it with the previous gidit
        while (factor > 1) {
            factor = factor/10;
            digit = tempNum/factor;
            newNum = newNum*digit;
            tempNum = tempNum%factor;
        }
        persistency++;
        number = newNum;
        newNum = 1;
        factor = 1;
        digit = 0;
    }   

    return persistency;
}

void getPersistent(int *array){
    int maxPersistentNumber = 0;
    int minPersistentNumber = 0;
    int maxPersistent = INT_MIN;
    int minPersistent = INT_MAX;
    
    //loops through list and finds the max and min persistent numbers
    for (int i=0; i<WORK_PORTION_PER_CHILD; i++) {
        int p = calculatePersistent(array[i]);
        if (p >= maxPersistent) { 
            maxPersistentNumber = array[i];
            maxPersistent = p;
        }
        
        if (p <= minPersistent) {
            minPersistentNumber = array[i];
            minPersistent = p;
        }
    }

    printf("Max Persitence number: %d with %d persistence from pid: %d\n", maxPersistentNumber, maxPersistent, getpid());
    printf("Min Persitence number: %d with %d persistence from pid: %d\n", minPersistentNumber, minPersistent, getpid());
}

void swap(int *array, int indexK, int indexJ) {
    int temp = array[indexK];
	array[indexK] = array[indexJ];
    array[indexJ] = temp;
}

/*
* Quicksort
*/
void sort(int *arr, int a, int b) {
    if(a >= b) {
        return;
    }
    int pivot = arr[b];
    int l = a;
    int r = b - 1;

    while(l <= r) {
        while(arr[l] < pivot) {
            l = l + 1;
        }

        while(arr[r] > pivot) {
            r = r - 1;
        }

        if(l <= r){
            swap(arr, l, r);
            l = l + 1;
            r = r - 1; 
        }
    }
    swap(arr, l, b);
    sort(arr, a, r);
    sort(arr, l, b);
}

bool readFile(int *array, char *file[]) {
    FILE *fp;
    
    //Open file
    if((fp = fopen(file[1], "r")) == NULL) {
        printf("Error opening file\n");
        return false;
    }
    
    //Reads the numbers into the array
    int n=0;
    while(fscanf(fp, "%d", &array[n]) == 1) n++;
    fclose(fp);
    return true;
}

bool argumentCheck(int argc) {
    if (argc != 2) {
        printf("Usage: PersistentNumber [fileName].txt\n");
        return false;    
    }

    return true;
}

/*
    Handles the signals for the IPC 
*/
void signalHandlerChild(int signo) {
}
