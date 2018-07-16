#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "pcbInitialize.c"
#include "decodeExec.c"

// define max. no. of processes
#define MAX 5

// for storing the number of instructions left in particular file
double noOfTimes[MAX];

// for switching threads	
int turn = 0;

// file pointer of logs_rr_with_thread file
FILE *fplog;

// choose the next thread to run
void SwitchThread(int tid)     
{
    int i;
    for(i = (tid + 1) % MAX; noOfTimes[i] == 0; i = (i + 1) % MAX)
        // if every thread has finished
        if(i == tid)    
            return;

    // switching threads
    turn = i;
}

// function which will be executed when thread starts execution
void * thread_execution(void * pid)
{
	int i = (int)pid;
	int result;
	char line[BUFSIZ];
	int j,cnt2=0;

	FILE *fptr=fopen(p_queue[i].p_name,"r");
	
	while(noOfTimes[i] != 0)
    {
    	// thread is busy waiting till its turn comes
        while(turn != i); 

        if(noOfTimes[i] > time_slice)
        {
        	// will execute till time_slice comes
        	cnt2=time_slice;		

			while(cnt2--)
			{
				// print process execution trace in log file
				fprintf(fplog, "\nProcess name: %s\n", p_queue[i].p_name); 
				// printf("\nProcess name: %s\n", p_queue[i].p_name);
				
				// fetch a line from file of process currently under execution
				fgets(line,sizeof(line),fptr);
					
	        	decodeExec(line,fplog); // calling "decodeExec" function
	        	p_queue[i].insNo++; // updating the number of instructions of process executed 
		            
		        // sleep(0.5); // sleep is to simulate the actual running time
			}        	
        		
	        noOfTimes[i]-=time_slice; // subtracting the number of instructions executed in this loop
        }

        // this thread will have finished after this turn
        else if(noOfTimes[i] > 0 && noOfTimes[i] <= time_slice)
        {
        	while(p_queue[i].insNo!=p_queue[i].cpu_burst_time)
        	{
        		// print process execution trace in log file
				fprintf(fplog, "\nProcess name: %s\n", p_queue[i].p_name); 
				// printf("\nProcess name: %s\n", p_queue[i].p_name);

        		// fetch a line from file of process currently under execution
				fgets(line,sizeof(line),fptr);
				
        		decodeExec(line,fplog); // calling "decodeExec" function
        		p_queue[i].insNo++; // updating the number of instructions of particular file executed 
	                     
	            // sleep((unsigned int)noOfTimes[i]); // sleep is to simulate the actual running time
        	}
	        noOfTimes[i] = 0; // to terminate the loop
        }
        fprintf(fplog, "\n----------\n");
        SwitchThread(i); // calling SwitchThread function
    }
    pthread_exit(0); // exiting Thread
}

int mainFunc(int ch)
{
	// initialize PCB for all processes
	pcbInitialize(ch);

	if(ch==1)
	{
		fplog=fopen("logs_rr_with_thread","a");
		pthread_t threads[MAX];
	    int i, status;

	    for(i=0; i<MAX; i++)
	        noOfTimes[i] = p_queue[i].cpu_burst_time; // input the burst time of each thread

	    for(i=0; i<MAX; i++)
	    {
	    	// create 1 thread for each process
	        status=pthread_create(&threads[i], NULL, thread_execution, (void *)i); 

	        // check if error in thread creation
	        if(status!= 0)
	        {
	            printf("While creating thread %d, pthread_create returned error code %d\n", i, status);
	            exit(-1);
	        }       
	    }

	    for(i=0;i<MAX;i++)
	    	pthread_join(threads[i], 0); // terminate the main program only after all threads terminate

	    fprintf(fplog, "\n<--- END of Log file --->");
	    fclose(fplog);

	    printf("\nLog file 'logs_rr_with_thread' generated!\n");
	}
	else
	{
		// file pointer of 'logs_rr_without_thread' file
		FILE *fplog=fopen("logs_rr_without_thread","a");

		// variables declaration
		int i, j, result;
		char line[BUFSIZ];

		// array denoting if a process has completed
		int checkProcessCompleted[MAX];

		// counter to check no. of completed processes
		int counter; 

		for(i=0;i<MAX;i++)
			checkProcessCompleted[i]=0;

		i=0, j=0;

		// gets us to the lineNo from where we have to resume in the process file
		int lineNo; 

		// loop runs process no. of times
		while(1)
		{
			// reset time slice value for every process
			time_slice=2; 

			// if all processes have completed execution
			if(counter==MAX)
				break;

			// if 'i' has reached MAX, bring it back in range of 0-4
			else if(i==MAX)
				i=0;

			else
			{	
				// if process has already been completed, move on to the next process
				if(checkProcessCompleted[i]==1)
				{
					i++;
					continue;
				}

				// open the process file
				FILE *fptr=fopen(p_queue[i].p_name,"r");

				// move to the instruction from where execution is to be resumed
				lineNo=1;
				while(lineNo<=p_queue[i].insNo)
				{
					fgets(line,sizeof(line),fptr);
					lineNo++;
				}

				// execute till time slice exhausts or process is completed
				for(j=0;j<time_slice && j<(p_queue[i].cpu_burst_time);j++)
				{
					// write to log file
					fgets(line, sizeof(line), fptr);
					p_queue[i].insNo++;
					if(strlen(line)<10)
						continue;
					fprintf(fplog, "\nProcess name: %s\n", p_queue[i].p_name);
					// decode and executed instruction fetched from the process file
					decodeExec(line, fplog);

					// if instruction is executed, make checkProcessCompleted entry '1'
					if(p_queue[i].insNo==p_queue[i].cpu_burst_time)
					{
						checkProcessCompleted[i]=1;
						counter++;
					}
				}
				i++; // go to the next process
				fclose(fptr);
				fprintf(fplog, "\n----------\n");
			}
		}
		fprintf(fplog, "\n<--- END of Log file --->");
		fclose(fplog);

		printf("\nLog file 'logs_rr_without_thread' generated!\n");
	}
	
    return 0;
}