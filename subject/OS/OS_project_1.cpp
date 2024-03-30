#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_PROCESSES 8
#define MAX_VALUE 1000
#define RANGE (MAX_VALUE / NUM_PROCESSES)

void process_task(int start, int end) {
    for (int i = start; i <= end; i++) {
        printf("%d ", i * 7);
    }
    printf("\n");
}

int main() {
    pid_t pid;
    int start = 1;
    int end = RANGE;
    clock_t start_time, end_time;
    double multi_process_time, single_process_time;

    // Start timing for multi-processes
    start_time = clock();

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();

        if (pid == 0) { // Child process
            int child_start = start + (i * RANGE);
            int child_end = (i == (NUM_PROCESSES - 1)) ? MAX_VALUE : (start + ((i + 1) * RANGE) - 1);
            process_task(child_start, child_end);
            exit(0);
        } else if (pid < 0) { // Error
            fprintf(stderr, "Fork failed!\n");
            return 1;
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // Calculate and print multi-process time
    end_time = clock();
    multi_process_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("Multi-processes time: %.2f milliseconds\n", multi_process_time);

    // Start timing for single process
    start_time = clock();

    // Single process
    for (int i = 1; i <= MAX_VALUE; i++) {
        printf("%d ", i * 7);
    }
    printf("\n");

    // Calculate and print single process time
    end_time = clock();
    single_process_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("Single process time: %.2f milliseconds\n", single_process_time);

    printf("Multi-P and Single-P's Time Difference: %.2f milliseconds\n", multi_process_time - single_process_time);
    printf("Number of processes: %d\n", NUM_PROCESSES);

    return 0;
}
