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

#define SIZE_OF_ARRAY 10

bool argumentCheck(); //check if any args were provided
bool readFile(int *array, char *file[]); //reads the file for list of numbers
void sort(int *arr, int a, int b); //sorts the input 
void calculatePersitent(); //calculates the persistency of the number


int main(int argc, char *argv[]) {  
    int numbers[SIZE_OF_ARRAY];   //array to hold the list of numbers
    
    if (!argumentCheck(argc) || !readFile(numbers, argv)) exit(0);
    
    for(int i=0;i<10;i++) printf("%d\n", numbers[i]); 
    
    sort(numbers, 0, 9);
    printf("\n");
    for(int i=0;i<10;i++) printf("%d\n", numbers[i]); 
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
        printf("Base case reached, returning\n");
        return;
    }
    int pivot = arr[b];
    int l = a;
    int r = b - 1;

    while(l <= r) {
        while(l <= r && arr[l] <= pivot) {
            l = l + 1;
        }

        while(l <= r && arr[l] >= pivot) {
            r = r - 1;
        }

        if(l < r){
            swap(arr, l, r);
        }
    }
    swap(arr, l, b);
    sort(arr, a, l - 1);
    sort(arr, l + 1, b);
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


