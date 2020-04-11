#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{

    int sum = 0;
    int i = 0;
    int processId = 0;

    fork();

    processId = (int) getpid();

    for(int i = 1; i<=100; i++){
        printf("%d (%d)\n", i, processId);
        fflush(stdout);
        sum += i;
    }

    printf("The sum is %d\n", sum);
    exit(0);
    return 0;
}
