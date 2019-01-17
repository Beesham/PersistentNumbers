/*
* Author: Beesham Sarendranauth | 104854956
* Date: 2019/01/24
* Class: Operating Systems Fundamentals (COMP3300)
* Title: PersistentNumbers.c (Homework #1) 
* Description: This program takes a file with a list of numbers
* that. The persistency of each number will be calculated and the max and min persitent will 
* be printed. The program will also calulate the execution time 
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

bool argumentCheck(); //check if any args were provided

int main(int argc, char *argv[]) {
    
    if (!argumentCheck(argc)) exit(0);
    
}

bool argumentCheck(int argc) {
    if (argc != 2) {
        printf("Usage: PersistentNumber [fileName].txt\n");
        return false;    
    }

    return true;
}


