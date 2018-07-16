#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "mainFunc.c"

int main()
{
	printf("<--- OS Project: Context Switching --->\n\n");

	printf("MENU\n");
	printf("1. Implementation with threads\n");
	printf("2. Implementation without threads\n");

	int ch;

	printf("\nEnter choice: ");
	scanf("%d", &ch);

	mainFunc(ch);
	
	return 0;
}