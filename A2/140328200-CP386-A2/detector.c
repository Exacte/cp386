/*
detector.c

Student Name : Mason Cooper
Student ID # : 140328200

*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 1000

void detector(FILE *fd){
    char line_buffer[MAX_LINE_LENGTH];
    char *token;
    
    //process input
    if (fgets(line_buffer, MAX_LINE_LENGTH, fd) != NULL && line_buffer[0] != '\n') {
        int n, m;
        //Process's first line
        sscanf(line_buffer, "%d %d",&n, &m);

        struct Process {
            int request[m];
            int allocated[m];
            int runnable;
            int completed;
        };
        struct Process processes[n];
        int unallocated[m];

        int i = 0;
        while (fgets(line_buffer, MAX_LINE_LENGTH, fd) != NULL && line_buffer[0] != '\n'){
            token = strtok(line_buffer, " ");

            //proces remaining line's of input
            if (i < n) {
                sscanf(token, "%d", &processes[i].request[0]);
                for (int j = 1; j < m; j++) {
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &processes[i].request[j]);
                }
                processes[i].completed = 0;
                processes[i].runnable = 0;
            } else if (i < 2*n) {
                sscanf(token, "%d", &processes[i-n].allocated[0]);
                for (int j = 1; j < m; j++) {
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &processes[i-n].allocated[j]);
                }
            } else {
                sscanf(token, "%d", &unallocated[0]);
                for (int j = 1; j < m; j++) {
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &unallocated[j]);
                }
            }
            i++;
        }

        //deadlock detection and correction
        int d_processes[n];
        int terminated[n];
        int execution_sequence[n];

        int systemEmpty = n;
        int d_index = 0;
        int t_index = 0;
        int es_index = 0;

        int deadlocked = 0;
        int output_type = 0;

        while (systemEmpty > 0) {
            deadlocked = 1;
            for (int i = n-1; i >= 0; i--) {
                if (processes[i].completed == 0) {
                    processes[i].runnable = 1;
                    for (int j = 0; j < m; j++) {
                        if (unallocated[j] < processes[i].request[j]) {
                            processes[i].runnable = 0;
                            break;
                        }
                    }

                    if (processes[i].runnable) {
                        deadlocked = 0;
                        processes[i].completed = 1;
                        for (int j = 0; j < m; j++) {
                            unallocated[j] += processes[i].allocated[j];
                        }
                        execution_sequence[es_index] = i+1;
                        es_index += 1;
                        systemEmpty = systemEmpty - 1;
                        if (systemEmpty != 0) {
                            i = n;
                        }
                    }
                }
            }

            if (deadlocked) {
                int terminate = 0;
                for (int i = 0; i < n; i++) {
                    if (processes[i].completed == 0 && processes[i].runnable == 0) {
                        if (!output_type) {
                            d_processes[i] = i+1;
                            d_index += 1;
                        }
                        terminate = i;
                    }
                }
                output_type = 1;

                terminated[t_index] = terminate+1;
                processes[terminate].completed = 1;
                for (int j = 0; j < m; j++) {
                    unallocated[j] += processes[terminate].allocated[j];
                }
                t_index += 1;
                systemEmpty = systemEmpty - 1;
            }
        }

        //print results
        if (output_type) {
            for (int i = 0; i < d_index; i++) {
                printf("%d ", d_processes[i]);
            }
            printf("\n");
            for (int i = 0; i < t_index; i++) {
                printf("%d ", terminated[i]);
            }
            printf("\n");
        }
        for (int i = 0; i < es_index; i++) {
            printf("%d ", execution_sequence[i]);
        }
        printf("\n");
    }
}