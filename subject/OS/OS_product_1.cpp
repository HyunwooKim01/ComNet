#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    
    int x = 1;
    pid_t pid= fork();           // fork(): 프로세스생성함수
    
    if (pid== 0) {
        printf("Child has x = %d\n", ++x);
    } else {
        printf("Parent has x = %d\n", --x);
    }
    
    printf("Bye for process %d with x = %d\n", getpid(), x);

}