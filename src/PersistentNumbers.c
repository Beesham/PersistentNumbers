/*
* Author: Beesham Sarendranauth | 104854956
* Date: 2019/01/24
* Class: Operating Systems Fundamentals (COMP3300)
* Title: PersistentNumbers.c (Homework #1) 
* Description: This program takes a file with a list of numbers
* that. The persistency of each number will be calculated and the max and min persitent will 
* be printed. The program will also calulate the execution time 
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

#define SIZE_OF_ARRAY 10

bool argumentCheck(); //check if any args were provided
void getPersistent(int *array); //calculates the persistency of the number
bool readFile(int *array, char *file[]); //reads the file for list of numbers
void sort(int *arr, int a, int b); //sorts the input 

int main(int argc, char *argv[]) {  
    int numbers[SIZE_OF_ARRAY];   //array to hold the list of numbers
 
    double executionTime = 0;
    clock_t startTime = clock();
    
    if (!argumentCheck(argc) || !readFile(numbers, argv)) exit(0);
    sort(numbers, 0, 9);
    getPersistent(numbers);    

    clock_t endTime = clock();
    executionTime += (double)(endTime - startTime) / CLOCKS_PER_SEC;

    printf("Execution time: %f seconds", executionTime);

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

    printf("Persistency of %d is %d\n", a, persistency);
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


