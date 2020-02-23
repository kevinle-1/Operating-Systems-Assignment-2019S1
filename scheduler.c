/**
 * AUTHOR: Kevin Le - 19472960
 * DATE: 21/04/2019
 * 
 * FILE: scheduler.c
 * PURPOSE: Main scheduler program demonstrating solution to the bounded buffer producer-consumer problem 
 *       with Posix Pthreads
 */

#include "scheduler.h"
#include "queue.h"

queue* q; 

/**
 * IMPORTS: Command line argument array and count
 * ASSERTION: Spawn 3 CPU threads and 1 Task thread, then joins the threads to complete execution.
 *            Appends final log data, containing average wait time and turnaround time.  
 */
int main(int argc, char* argv[])
{
    if(argc != 3) //Check sufficient arguments 
    {
        printf("Error: Insufficient Arguments\n");
        return -1; //Exit
    }
    else if(atoi(argv[2]) > 10 || atoi(argv[2]) < 1) //Check allowed queue size
    {
        printf("Incorrect Queue Size!\n");
        return -1; //Exit
    }

    int i, j, k; //For loops
    double avgWait, avgTAround; //Store real values of average calculations
    char logLine[256]; //Line to write to simulation_log

    pthread_t cpuTID[3]; //Thread IDs of CPU threads
    pthread_t taskTID; //Thread ID of Task thread 

    strcpy(fileName, argv[1]); 

    q = newQueue(atoi(argv[2])); //Initialize queue
    if(!getNumTasks())
    {
        printf("Error Opening File!\n");
        return -1; 
    }

    for(i = 0; i < 3; i++) //Create 3 CPU threads
    {
        pthread_create(&cpuTID[i], NULL, cpu, NULL);
    }

    pthread_create(&taskTID, NULL, task, NULL); //Create 1 Task thread

    c1 = cpuTID[0]; //Store CPU IDs in global ints for getCPUID() 
    c2 = cpuTID[1]; 
    c3 = cpuTID[2];

    pthread_join(taskTID, NULL); //Join task thread. (Wait for it to complete exec)
    printf("Task Thread Finished\n");

    pthread_cond_broadcast(&qElement); 
    
    for(j = 0; j < 3; j++) //Join CPU threads. (Wait for it to complete exec)
    {
        pthread_join(cpuTID[j], NULL);
        printf("CPU Thread %d Finished\n", j+1);
    }

    avgWait = total_waiting_time/(double)num_tasks; //Calculate average waiting time 
    avgTAround = total_turnaround_time/(double)num_tasks; //Calculate average turnaround time 

    sprintf(logLine, "Number of tasks: %d\nAverage waiting time: %.2f seconds\nAverage turn around time: %.2f seconds\n", num_tasks, avgWait, avgTAround);
    writeLog(logLine); //Write final statistics to simulation_log

    free(q->array);
    free(q);
}

/**
 * IMPORTS: N/A
 * EXPORTS: Void pointer. 
 * ASSERTION: Producer/Writer component. 
 *            Opens file, obtains lock on queue, then checks if queue is full - if full waits for Consumer/Reader.
 *            to empty. Then enqueues 2 tasks at a time, writing arrival time to log. Then broadcasts the condition
 *            qElement to all readers. Waits for reader to service task before continuing. Finishes once all tasks 
 *            from file added. 
 */ 
void* task()
{
    char line[10];

    int taskN, cpuB, j; 
    int totalQueued = 0; //Total elements enqueued (so far). 

    time_t curTime; //Current time 

    char timeStr[256]; //Current time represented as a string 
    char termTime[256]; //Termination time represented as a string 

	char logLine[256]; //Line 1 to write to simulation_log
	char logLine2[256]; //Line 2 to write to simulation_log

    int finished = FALSE; //Variable modified by the writer to signal if writer has finished 

    FILE* taskFile = fopen(fileName, "r");
    if(taskFile == NULL) //Check open file success 
    {
        printf("Error opening file!\n");
        finished = TRUE; 
        return (void*)-1;
    }

    while(!finished)
    {
        pthread_mutex_lock(&mutex); //Get lock on queue
        
        while(isFull(q)) //While the queue is full 
        {
            pthread_cond_wait(&qEmpty, &mutex); //Wait for the readers to empty the queue (signal empty)
        }

        if(totalQueued < tasksInFile) //If there are still lines to be read from the file 
        {
            if(totalQueued == tasksInFile-val) //Condition to stop CPU threads once last task has been added
            {
                lastTask = TRUE; 
            }

            for(j = 0; j < 2; j++)
            {
                if(!(isFull(q))) 
                {
                    if(fgets(line, 10, taskFile) != NULL) //Read from the file and add to queue
                    {
                        if(sscanf(line, "%d %d", &taskN, &cpuB) > 1)
                        {
                            time(&curTime); //Current time 
                            strftime(timeStr, 256, "%H:%M:%S", localtime(&curTime)); //Format current time 

                            enqueue(q, taskN, cpuB); //Enqueue task  

                            sprintf(logLine, "%d:%d\nArrival Time: %s", taskN, cpuB, timeStr); //format line for log file 
                            writeLog(logLine); //Write to logfile 

                            totalQueued++;
                        }
                    }
                }
            }

            pthread_cond_broadcast(&qElement); //Signal to all readers that elements now exist in queue
            pthread_cond_wait(&qServiced, &mutex); //Wait for readers to service the element/s - Required to enqueue 2 task at a time 
        }
        else
        {
            finished = TRUE; //Stop writing as entire file has been read. 
        }
        pthread_mutex_unlock(&mutex); //Release lock on queue 
    } 

    time(&curTime); //Current time 
    strftime(termTime, 256, "%H:%M:%S", localtime(&curTime)); //Format current time 

    sprintf(logLine2, "Number of tasks put into Ready-Queue: %d\nTerminated at time: %s", totalQueued, termTime);
    writeLog(logLine2); //Write total tasks added and termination time to log 

    fclose(taskFile); 
}

/**
 * IMPORTS: N/A
 * EXPORTS: Void pointer. 
 * ASSERTION: Consumer/Reader component. Obtains lock on queue, checks if queue is empty, if it is then waits 
 *            for task() to signal condition qElement. Dequeues task and sleeps for its CPU burst. Writing the
 *            arrival, service and completed times to log. Then signals condition qServiced.
 */  
void* cpu()
{
    time_t curTime; 
    int serviced = 0; 

    char arrivTime[256]; //Arrival time as string
    char servTime[256]; //Served time as string
    char compTime[256]; //Completed time as string

    char logLine[256]; //Line 1 to write to simulation_log
    char logLine2[256]; //Line 2 to write to simulation_log
    char logLine3[256]; //Line 3 to write to simulation_log

    int cID; //CPU ID

    while(!lastTask) //Continue if last task has not been serviced
    {
        pthread_mutex_lock(&mutex); //Obtain lock on queue 

        cID = getCPUID((int)pthread_self()); //Retrieve CPU ID based on thread ID
        
        while(isEmpty(q)) //If the queue is empty 
        {
            pthread_cond_signal(&qEmpty); //Signal that it is empty 
            pthread_cond_wait(&qElement, &mutex); //Wait for task() to signal that element exists. 
        }

        tsk* t = dequeue(q); //Dequeue element

        num_tasks++; //Global task counter
        serviced++; //Individual thread task counter 

        strftime(arrivTime, 256, "%H:%M:%S", localtime(&t->arrivalTime)); //Retrieve and convert arrival Time from task struct  

        time(&curTime); //Get current time 
        strftime(servTime, 256, "%H:%M:%S", localtime(&curTime)); //Format current time 

        sprintf(logLine, "Statistics for CPU-%d:\nTask %d\nArrival time: %s\nService time: %s", cID, t->taskNum, arrivTime, servTime);
        writeLog(logLine); //Write service time stats

        total_waiting_time = total_waiting_time + difftime(curTime, t->arrivalTime); //Increment total_waiting_time by wait time for task

        printf("T: %d C: %d\n", t->taskNum, t->cpuBurst); 
        sleep(t->cpuBurst); //Simulate cpuBurst with sleep 

        time(&curTime); //Get current time again 
        strftime(compTime, 256, "%H:%M:%S", localtime(&curTime)); //Format new current time 
        sprintf(logLine2, "Statistics for CPU-%d:\nTask %d\nArrival Time: %s\nCompletion Time: %s", cID, t->taskNum, arrivTime, compTime);
        writeLog(logLine2); //Write completion time stats

        total_turnaround_time = total_turnaround_time + difftime(curTime, t->arrivalTime); //Increment total_turnaround_time by turnaround time for task

        pthread_cond_signal(&qServiced); //Signal that task has been serviced
        pthread_mutex_unlock(&mutex); //Release lock on queue 
    }

    sprintf(logLine3, "CPU-%d terminates after servicing %d tasks", cID, serviced); 
    writeLog(logLine3); //Write CPU stats on thread finish. 
}

/**
 * IMPORTS: N/A
 * ASSERTION: Retrieves number of tasks from file. 
 */ 
int getNumTasks()
{
    FILE* taskFile = fopen(fileName, "r");
    char line[10];
    //Count num tasks 
    if(taskFile != NULL)
    {
        while(fgets(line, 10, taskFile) != NULL) 
        {
            tasksInFile++; //Increase task count. 
        }

        fclose(taskFile);
    }
    else
    {
        return 0; 
    }

    if(tasksInFile % 2 == 0)
    {
        val = 4; //!Skips final value from even numbered file. 
    }
    else
    {
        val = 3;
    }

    return 1; 
    //Let writer exit on error.
}

/**
 * IMPORTS: Integer representing thread ID
 * EXPORTS: Integer representing CPU ID (1/2/3)
 * ASSERTION: Compares thread ID with c1,c2,c3 values in global to return CPU number that is executing. 
 */ 
int getCPUID(int TID)
{
    if(TID == c1)
    {
        return 1;
    }
    else if(TID == c2)
    {
        return 2;
    }
    else if(TID == c3)
    {
        return 3;
    }
}


/**
 * IMPORTS: Formatted string (sprintf) to write to simulation_log
 * ASSERTION: Opens simulation_log for appending, writes formatted log string and seperates each log write with --
 */ 
void writeLog(char* logLine)
{
	FILE* logFile = NULL;
	logFile = fopen("simulation_log", "a");
    if(logFile != NULL)
    {
        fprintf(logFile, strcat(logLine, "\n--\n"));
        fclose(logFile); 
    }
    else
    {
        printf("Error Opening Log File!\n");
    }
}