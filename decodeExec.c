#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

void decodeExec(char instruction[], FILE *fplog)
{
	fprintf(fplog, "Instruction: %s", instruction);

	char *ch;

	// maximum no. of operatin name is 10 and each operation name length is BUFSIZ
	char insParams[10][BUFSIZ];

	// opcode op1 op2 - 3 tokens in instruction - store in array
	int i=0,j=0,k=0,l=0;

	// split 'instruction' into series of tokens with delimiter ' ' - store all tokens in "insParams" array
	ch=strtok(instruction," ");
	while (ch != NULL) 
	{
		strcpy(insParams[i],ch); // store the token in 'insParams' array
		ch=strtok(NULL," "); // fetch the next token
		i++;
	}

	// tempT contains insParams[3] without '\n' character at end
	char tempT[BUFSIZ];
	for(l=0;insParams[3][l]!='\n';l++)
	{ 
		tempT[l]=insParams[3][l];
	}
	tempT[l] = '\0';

	// At this point, insParams contains the operands in form of tokens
	char operation[4];

	// copy instruction code to char array 'operation'
	strcpy(operation,insParams[0]); 

	// open opcodes.txt since we need to find opcode of the instruction
	FILE *fin;
	fin=fopen("variables.txt", "r");

	// store 1 line data from 'variables.txt' file in 'line' variable
	char line[BUFSIZ];

	char var1[BUFSIZ], var2[BUFSIZ];
	int val1,val2;
	int var1Found=0, var2Found=0;

	// fetch 1 line from "variables.txt" file
	while(fgets(line,sizeof(line),fin) && (!(var1Found && var2Found)))
	{	
		// get the first token
   		ch = strtok(line, " ");
   		if(strcmp(ch,insParams[1])==0)
   		{
   			var1Found=1;

   			//get the next token
   			ch = strtok(NULL, " ");
   			strcpy(var1, ch);
   		}

   		if(strcmp(ch,insParams[2])==0)
   		{
   			var2Found = 1;

   			//get the next token
   			ch = strtok(NULL, " ");
   			strcpy(var2, ch);
   		}
	}

	// convert values of val1 and val2 to int as they are of type 'string'
	// if var1 was not found meaning there was an integer passed in place of var1
   	if(!var1Found)
   	{
   		val1=atoi(insParams[1]);
   	}
   	else
   	{
   		val1=atoi(var1);
   	}

   	// if var2 was not found meaning there was an integer passed in place of var2
   	if(!var2Found)
   	{
   		val2=atoi(insParams[2]);
   	}
   	else
   	{
   		val2=atoi(var2);
   	}
   	fclose(fin); // close "variables.txt" file

	int result;

	// --- operations are also given a code - to fetch opcodes --- //
	// open opcodes.txt since we need to find opcode of the instruction
	fin=fopen("opcode.txt", "r");

	// array to store all opcodes; max. no. of opcodes taken as 10; pointer because opcode
	char opNames[10][4]; 
	int opCodes[10];

	i=0;

	// store all opcodes in "opcodes[]"
	while(fgets(line,sizeof(line),fin))
	{	
		// get the first token
   		ch = strtok(line, " ");
   		strcpy(opNames[i], ch);

   		//get the next token
   		ch = strtok(NULL, " ");
   		
   		opCodes[i]=atoi(ch);

   		i++;
	}

	int finalOpCode;
	for(j=0;j<5;j++)
	{
		if(strcmp(opNames[j], operation)==0)
		{
			finalOpCode=opCodes[j];
		}
	}
	fclose(fin);

	fprintf(fplog, "Decoded instruction: %d %d %d %s\n", finalOpCode, val1, val2, tempT);

	FILE *fr, *fw;
	char temp[BUFSIZ], temp1[BUFSIZ];

	// perform execution according to opcode
	switch(finalOpCode)
	{
		case 1: // add
		result=val1+val2;
		break;

		case 2: // sub
		result=val1-val2;
		break;

		case 3: // mul
		result=val1*val2;
		break;

		case 4: // div
		result=val1/val2;
		break;

		case 5: // mod
		result=val1%val2;
		break;
	}

	// open "variables.txt" for reading and a new variables file for writing
	fr=fopen("variables.txt", "r");
	fw=fopen("variablesNew.txt", "w");

	int flag=0;
	while(fgets(line,sizeof(line),fr))
	{	
		strcpy(temp1,line);
		ch=strtok(line, " ");

		// if result variables' value is to be modified
		if(strcmp(ch, tempT)==0) 
		{
			flag=1;
			strcpy(temp,strcat(ch," ")); 
			char resultStr[BUFSIZ];

			// convert integer value of result to string
			sprintf(resultStr, "%d", result);
			strcat(temp,resultStr);

			// write modified value in "variablesNew.txt" file
			fputs(strcat(temp,"\n"),fw);
		}

		// if result variables' value is not to be modified
		else
		{
			fputs(strcat(temp1,""),fw);
		}
	}
	fclose(fw);		
	fclose(fr);

	// perform file operations using shell commands
	// remove old "variables.txt" file
	system("cd $PWD");
	system("rm variables.txt");

	// rename "variablesNew.txt" file to "variables.txt" file
	system("mv variablesNew.txt variables.txt");

	// if result variable doesn not exist in "variables.txt" file, store it in the file
	if(flag==0)
	{
		fw=fopen("variables.txt","a");
		strcpy(temp,strcat(tempT, " ")); 

		// convert integer value of result to string		
		char resultStr[BUFSIZ];
		sprintf(resultStr, "%d", result);
		strcat(temp,resultStr);

		// write the result variable name and its value to "variables.txt" file
		fputs(strcat(temp,"\n"),fw);
		fclose(fw);
	}

	fprintf(fplog, "%s: %d\n", tempT, result);
}