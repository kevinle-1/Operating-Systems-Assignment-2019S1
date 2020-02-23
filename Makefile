#AUTHOR: Kevin Le - 19472960
#DATE: 21/04/2019
#TASK: Makefile to compile scheduler program

CC = gcc
EXEC = scheduler
T = -lpthread

OBJ = sched.o queue.o 
CFLAGS = -g -Werror 

#--------PROGRAM-EXECUTEABLES-----------------------------------------

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(T)

#--------OBJECTS------------------------------------------------------

sched.o: scheduler.c 
	$(CC) -c $(CFLAGS) scheduler.c -o sched.o $(T)

queue.o: queue.c 
	$(CC) -c $(CFLAGS) queue.c -o queue.o 

#--------UTILS--------------------------------------------------------

clean:
	rm -f $(OBJ) scheduler simulation_log
	clear
