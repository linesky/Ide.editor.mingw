#include <stdio.h>
#include <stdlib.h>

int main(){
    char c[1024];
    printf("hello world....\n");
    fgets(c,1023,stdin);
    return 0;
}