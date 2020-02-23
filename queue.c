/**
 * AUTHOR: Kevin Le - 19472960
 * DATE: 21/04/2019
 * 
 * FILE: queue.c
 * PURPOSE: FIFO Queue implemented with dynamic array for use by scheduler.c 
 * 
 * REFERENCES: Implementation based on "Queue | Set 1 (Introduction and Array Implementation)" 
 *             - Geeks for Geeks, Accessed 18/04/2019. Retrieved from: 
 *             https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
 */

#include "queue.h"

/*Creates queue that can hold ints. Initially empty*/

/**
 * IMPORTS: Integer representing queue capacity retrieved from "m" cmdline in scheduler.
 * ASSERTION: Creates a new queue of inCapacity size, mallocing an array of task structs to represent queue. 
 */
queue* newQueue(int inCapacity)
{
    queue* q = (queue*)malloc(sizeof(queue));

    q->capacity = inCapacity;
    q->size = 0;
    q->front = q->size;
    q->rear = inCapacity - 1;

    q->array = (tsk*)malloc(q->capacity * sizeof(tsk));
}

/**
 * IMPORTS: Queue 
 * ASSERTION: Determines if queue is full 
 */
int isFull(queue* q)
{
    return(q->size == q->capacity);
}

/**
 * IMPORTS: Queue 
 * ASSERTION: Determines if queue is empty 
 */
int isEmpty(queue* q)
{
    return(q->size == 0);
}

/**
 * IMPORTS: Queue, Integer task number from file, Integer cpu burst from file  
 * ASSERTION: Handles creation of the task struct on the stack, calculating its arrival time 
 *            (time it is inserted) storing the time into the task struct. Then inserting into queue. 
 */
void enqueue(queue* q, int inTaskNum, int inCpuBurst)
{
    if(isFull(q))
    {
        printf("Queue Full!\n"); 
    }
    else
    {
        tsk t; //Create on stack 

        t.taskNum = inTaskNum;
        t.cpuBurst = inCpuBurst;
        t.arrivalTime = time(NULL); //Assign time_t value to arrivalTime field in tsk struct

        q->rear = (q->rear + 1) % q->capacity; 
        q->array[q->rear] = t; 
        q->size = q->size + 1; 
    }
}

/**
 * IMPORTS: Queue 
 * ASSERTION: Removes item from queue (FIFO Order) 
 */
tsk* dequeue(queue* q)
{
    if(isEmpty(q))
    {
        printf("Queue Empty!\n"); 
        return NULL;
    }
    else
    {
        
        tsk* t = &q->array[q->front];
        q->front = (q->front + 1) % q->capacity;
        q->size = q->size - 1; 

        return t;
    }
}

/**
 * IMPORTS: Queue 
 * ASSERTION: Returns value at front of queue  
 */
tsk* front(queue* q)
{
    if(isEmpty(q))
    {
        printf("Empty!");
    }
    else
    {
        return &q->array[q->front];
    }
    
}

/**
 * IMPORTS: Queue 
 * ASSERTION: Returns value at rear of queue  
 */
tsk* rear(queue* q)
{
    if(isEmpty(q))
    {
        printf("Empty!");
    }
    else
    {
        return &q->array[q->rear];
    }
    
}