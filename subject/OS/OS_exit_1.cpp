#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void cleanUp() {
    printf("Clean\n");
}

int main() {
    atexit(cleanUp);
    fork();
    exit(0);    //exit(): 프로세스 종료 함수
}