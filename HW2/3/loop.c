#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
        printf("%d\n", getpid());
        getchar();
        int i=0;
        int *ptr = malloc(33554432*sizeof(int));
        while(1)
        {
                *(ptr+i)=i;
                printf("%d\n", *(ptr+i));
                i++;
                if(i==33554432)
                {
                        i=0;
                }
        }
        /*while(1)
        {
                printf("looping.\n");
        }
        return 0;*/
}

