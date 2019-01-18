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

#define SIZE_OF_ARRAY 10

bool argumentCheck(); //check if any args were provided
bool readFile(); //reads the file for list of numbers
void sort(); //sorts the input 
void getPersistent(int *array); //calculates the persistency of the number

int main(int argc, char *argv[]) {  
    int numbers[SIZE_OF_ARRAY];   //array to hold the list of numbers
    
    if (!argumentCheck(argc) || !readFile(numbers, argv)) exit(0);
    getPersistent(numbers);    
}


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
    int minPersistentNumber = maxPersistentNumber;
    int maxPersistent = 0;
    int minPersistent = INT_MAX;
    
    //loops through list and finds the max and min persistent numbers
    for (int i=0; i<sizeof(array); i++) {
        int p = calculatePersistent(array[i]);
        if (p >= maxPersistent) { 
            maxPersistentNumber = array[i];
            maxPersistent = p;
        } else if (p <= minPersistent) {
            minPersistentNumber = array[i];
            minPersistent = p;
        }
    }

    printf("MaxP num: %d", maxPersistentNumber);
    printf("MinP num: %d", minPersistentNumber);
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
    while(fscanf(fp, "%d", &array[n]) != EOF) n++;
        
    return true;
}

bool argumentCheck(int argc) {
    if (argc != 2) {
        printf("Usage: PersistentNumber [fileName].txt\n");
        return false;    
    }

    return true;
}


