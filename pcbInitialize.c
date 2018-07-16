#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

//structure of process control block
struct pcb
{
    int p_id;
    double insNo;
    char p_name[50];
    double arrival_time;
    double cpu_burst_time;
    int priority;
};

int noOfFiles=0;
int time_slice=2;

// ready queue which has pcb of all processes
struct pcb p_queue[10];   

//main function which creates process
void pcbInitialize(int ch)
{
    // seed for random function - to generate random no. everytime
    srand(time(0)); 
    time_t t=time(0);

    // navigating to directory using shell commands
    /* system("cd processes");
    system("ls > lsout.txt");
    system("echo $PWD > dirPath.txt");

    FILE *fp=fopen("dirPath.txt", "r");
    while(fgets(line,sizeof(line),fp))
    {
        strcpy(temp,line);
    }
    fclose(fp);

    temp[strlen(temp)-1]='\0';
    printf("temp: %s\n", temp);
    FILE *fin=fopen("lsout.txt", "r");
    while(fgets(line, sizeof(line), fin))
    {
        strcpy(line1, "");
        line[strlen(line)-1]='\0';
        printf("line: %s\n", line);
        strcat(temp,"/");
        printf("temp Afterwards: %s\n", temp);
        strcpy(line1, strcat(temp, line));
        line1[strlen(line1)]='\0';
        printf("line1: %s\n\n", line1);
        strcpy(pname[noOfFiles], line1);
        printf("pname[noOfFiles]=%s", pname[noOfFiles]);
        noOfFiles++;
    }
    fclose(fin); */

    // fetching process files from "processes" directory
    struct dirent *dp;
    char pname[10][50];
    char folder[]="processes/";

    DIR *fd; // pointer to directry
    fd=opendir("processes"); // open processes directry

    while((dp = readdir(fd)) != NULL) 
    {
        // skip self and parent
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
            continue;    

        char  folder[]="processes/";
        strcat(folder,dp->d_name);

        // copy the relative file name in pname[i]
        strcpy(pname[noOfFiles],folder);
        
        // increment the index for "pname" array
        noOfFiles++; 
    }
    closedir(fd);

    FILE *f1, *fplog;
    double count=0;
    char c;
    int i;

    // write in log file
    // with threads
    if(ch==1)
    {
        fplog=fopen("logs_rr_with_thread","w");
        fprintf(fplog, "<--- START of Round Robin Log file (with Threads) --->\n\n");
        fprintf(fplog, "Time slice = %d instructions\n\nReady Queue:\n\nFront -> ", time_slice);
    }

    // without threads
    else
    {
        fplog=fopen("logs_rr_without_thread","w");
        fprintf(fplog, "<--- START of Round Robin Log file (without Threads) --->\n\n");
        fprintf(fplog, "Time slice = %d instructions\n\nReady Queue:\n\nFront -> ", time_slice);
    }

    // initializing the parameters of process in PCB
    for(i=0;i<noOfFiles;i++)
    {
        count=0;

        if(i==noOfFiles-1)
            fprintf(fplog, "%s\n\n----------", pname[i]);
        else
            fprintf(fplog,"%s | ",pname[i]);

        // store values in pcb parameters
        p_queue[i].p_id=(i+1);
        p_queue[i].insNo=0;
        strcpy(p_queue[i].p_name,pname[i]);
        p_queue[i].arrival_time=difftime(time(0),t);
        p_queue[i].priority=rand()%10;

        // calculate cpu burst time
        f1=fopen(pname[i],"r");
        for (c = getc(f1); c != EOF; c = getc(f1))
        {
            if (c == '\n')
                count = count + 1;
        }
        p_queue[i].cpu_burst_time=count;

        fclose(f1);
    }
    fprintf(fplog, "\n");
    fclose(fplog);
}