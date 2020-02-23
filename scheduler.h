/**
 * AUTHOR: Kevin Le - 19472960
 * DATE: 21/04/2019
 * 
 * FILE: scheduler.h
 * PURPOSE: Header file for scheduler.c
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

//Values required per assignment spec. Used to calculate averages. 
int num_tasks = 0; 
double total_waiting_time = 0; 
double total_turnaround_time = 0;

char fileName[15]; //Stores filename specified in commandline 

int tasksInFile = 0; //Number of tasks in the file, modified by the getNumTasks()
int val = 0; //When to finish CPU thread. 
int c1,c2,c3; //Stores CPU IDs 

int lastTask = FALSE; //Variable modified by the writer to signal if writer has enqueued the last task in the file  

pthread_cond_t qServiced = PTHREAD_COND_INITIALIZER; //Pthread condition to signal/ wait on if the task has been serviced 
pthread_cond_t qEmpty = PTHREAD_COND_INITIALIZER; //Pthread condition to signal/wait if queue is empty 
pthread_cond_t qElement = PTHREAD_COND_INITIALIZER; //Pthread condition to signal if 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Pthread condition to obtain a lock on the queue 

void* task(); 
void* cpu();

int getNumTasks();
int getCPUID(int TID);
void writeLog(char* line);