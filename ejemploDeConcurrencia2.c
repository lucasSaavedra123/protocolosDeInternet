#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int processId = 0;
    int newProcessId = 0;

    processId = (int) getpid();
    newProcessId = fork();

    printf("%d\n", processId);
    printf("%d\n", newProcessId);

    exit(0);
}
