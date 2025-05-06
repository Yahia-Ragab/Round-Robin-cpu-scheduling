#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void safeScanInt(const char *prompt, int *x) {
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin)) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        int valid = 1;
        for (int i = 0; buffer[i]; i++) {
            if (!isdigit(buffer[i])) {
                valid = 0;
                break;
            }
        }
        if (valid && strlen(buffer) > 0) {
            *x = atoi(buffer);
            if (*x >= 0) break;
        }
        printf("Wrong entry. Please enter a non-negative integer.\n");
    }
}

void findWaitingTime(int processes[], int n, int bt[], int wt[], int rt[], int quantum) {
    int rem_bt[n], visited[n];
    for (int i = 0; i < n; i++) {
        rem_bt[i] = bt[i];
        visited[i] = 0;
        wt[i] = 0;
        rt[i] = -1;
    }

    int t = 0;
    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = 0;
                if (rt[i] == -1) rt[i] = t;

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    wt[i] = t - bt[i];
                    rem_bt[i] = 0;
                }
            }
        }
        if (done) break;
    }
}

void findTurnAroundTime(int n, int bt[], int wt[], int tat[]) {
    for (int i = 0; i < n; i++)
        tat[i] = bt[i] + wt[i];
}

void findAverageTime(int processes[], int n, int bt[], int quantum) {
    int wt[n], tat[n], rt[n];
    findWaitingTime(processes, n, bt, wt, rt, quantum);
    findTurnAroundTime(n, bt, wt, tat);

    float total_wt = 0, total_tat = 0, total_rt = 0;

    for (int i = 0; i < n; i++) {
        total_wt += wt[i];
        total_tat += tat[i];
        total_rt += rt[i];
        printf("P[%d]\tBurst Time: %d,\tWaiting Time: %d,\tTurnaround Time: %d\t,Resonse Time: %d\n", processes[i], bt[i], wt[i], tat[i], rt[i]);
    }

    printf("\nAverage waiting time: %.2f\n", total_wt / n);
    printf("Average turnaround time: %.2f\n", total_tat / n);
    printf("Average response time: %.2f\n", total_rt / n);
}

int main() {
    int n, quantum;

    safeScanInt("Enter the number of processes: ", &n);

    int processes[n], bt[n];
    printf("Enter burst times for each process:\n");
    for (int i = 0; i < n; i++) {
        char prompt[20];
        sprintf(prompt, "P[%d]: ", i + 1);
        safeScanInt(prompt, &bt[i]);
        processes[i] = i + 1;
    }

    safeScanInt("Enter time quantum: ", &quantum);

    findAverageTime(processes, n, bt, quantum);

    return 0;
}
