/**
 * AUTHOR: Kevin Le - 19472960
 * DATE: 21/04/2019
 * 
 * FILE: queue.h
 * PURPOSE: Header file for queue.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
typedef struct tsk
{
    int taskNum;
    int cpuBurst;

    time_t arrivalTime; 
} tsk; 

typedef struct queue
{
    int front, rear, size; 
    unsigned capacity;
    tsk* array; 
} queue;

queue* newQueue(int inCapacity);

void enqueue(queue* q, int inTaskNum, int inCpuBurst);
tsk* dequeue(queue* q);

int isFull(queue* q);
int isEmpty(queue* q);

tsk* front(queue* q);
tsk* rear(queue* q);
