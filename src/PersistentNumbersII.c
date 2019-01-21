/*
* Author: Beesham Sarendranauth | 104854956
* Date: 2019/01/24
* Class: Operating Systems Fundamentals (COMP3300)
* Title: PersistentNumbersII.c (Homework #1) 
* Description: This program takes a file with a list of numbers
* that. The persistency of each number will be calculated and the max and min persitent will 
* be printed. The program will also calulate the execution time. 
* This version takes advantage of multithreding via multiple processes (fork) 
* Assumptions: a number is 9 digits long and is line separated
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

bool argumentCheck(); //check if any args were provided
void getPersistent(int *array); //calculates the persistency of the number
bool readFile(int *array, char *file[]); //reads the file for list of numbers
void sort(int *arr, int a, int b); //sorts the input 
int spawnChild(); //spawns a child process 
void signalHandlerChild(int signo);//Signal Handler for child processes
void signalHandlerParent(int signo);//Signal handler for parent proecess

//List of child pids the parent keeps as a queue
int childPids[MAX_CHILD_PROCESSES];
int FLAG_CONT = 0; //flag to notify weather or not to keep sending kills

int main(int argc, char *argv[]) {  
    int numbers[SIZE_OF_ARRAY];   //array to hold the list of numbers
    int childProcesses[MAX_CHILD_PROCESSES]; //array to hold child id 
    int childCount = 0;
    int pid;    

    //registers the signal handler for the parent process        
    signal(SIGUSR2, signalHandlerParent);

    double executionTime = 0;
    clock_t startTime = clock();
    
    if (!argumentCheck(argc) || !readFile(numbers, argv)) {
        clock();        
        exit(0);
    }
    
    sort(numbers, 0, 9);
    getPersistent(numbers);    

    clock_t endTime = clock();
    executionTime += (double)(endTime - startTime) / CLOCKS_PER_SEC;

    printf("Execution time: %f seconds\n", executionTime);

    for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
        if((pid = spawnChild(&childCount)) != 0) {
            childProcesses[i] = pid;
            childPids[i] = pid;
        } else break;
    }
    
    //do parent stuff
    if (pid > 0) {    
        printf("I am the father of the following: "); 
        for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
            if(i == MAX_CHILD_PROCESSES - 1) {
                printf("and %d", childProcesses[i]);
            } else printf("%d, ", childProcesses[i]);
        }
        printf("\n");

        while(FLAG_CONT == 0) {
            sleep(1);
            if(childPids[0] != '\0') {
                kill(childPids[0], SIGUSR1);
            } else {
                FLAG_CONT = 1;
            }
        }       

    }

    //do child stuff
    if (pid == 0) {
        signal(SIGUSR1, signalHandlerChild);
        pause();
        printf("I am kid #: %d with pid: %d\n", childCount, getpid());
        kill(getppid(), SIGUSR2);
    }
    return 0;
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
    for (int i=0; i<SIZE_OF_ARRAY; i++) {
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

    printf("Max Persitence number: %d with %d persistence\n", maxPersistentNumber, maxPersistent);
    printf("Min Persitence number: %d with %d persistence\n", minPersistentNumber, minPersistent);
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

/*
    Habdles the sidnals for parent proc
*/
void signalHandlerParent(int signo) {
    sleep(1);
    //Queue data structure     
    int sizeOfArr = sizeof(childPids) / sizeof(int);
    for(int i = 0; i < sizeOfArr; i++) {
        childPids[i] = childPids[i+1];   
    }
    childPids[sizeOfArr] = '\0';
    printf("processing...\n");
}
