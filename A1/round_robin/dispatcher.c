/*
dispatcher.c

Student Name : Mason Cooper
Student ID # : 140328200

Dispatch Algorithm : Round Robin
*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 100

#include "queue.h"

void dispatcher(FILE *fd, int quantum, int harddrive){
	struct Process {
		int start_time;
		int process_id;
		int run_time;
		int num_exchanges;
		int exchanges[50];

		int total_run_time;
		int total_ready_time;
		int total_blocked_time;
		int status;
		//process status 0 = blocked, 1 = ready, 2 = running, -1 = not in system(i.e in new queue or completed)
	};

	struct Queue *newQueue = createQueue();
	struct Queue *readyQueue = createQueue();
	struct Queue *blockedQueue = createQueue();

	struct Process idleProcess = {0, 0};

    char line_buffer[MAX_LINE_LENGTH];
    int start_time, run_time, process_id, num_exchanges, exchange_time;
    char *token;
    
    struct Process processes[98];
    int i = 0;
    int pid[99];

    //Process simulation input line by line
    while (fgets(line_buffer, MAX_LINE_LENGTH, fd) != NULL && line_buffer[0] != '\n'){

        token = strtok(line_buffer, " ");
        sscanf(token, "%d",&start_time);
       
        token = strtok(NULL, " ");
        sscanf(token, "%d",&process_id);
        
        token = strtok(NULL, " ");
        sscanf(token, "%d",&run_time);
        int m = process_id%99;
        processes[m].start_time = start_time;
        processes[m].process_id = process_id;
        processes[m].run_time = run_time;
        
        num_exchanges = 0;
        token = strtok(NULL, " ");
        while ( token != NULL ){
            num_exchanges += sscanf(token, "%d",&exchange_time);
            token = strtok(NULL, " ");
            processes[m].exchanges[num_exchanges - 1] = exchange_time;
        }
        processes[m].num_exchanges = num_exchanges;

        processes[m].total_run_time = 0;
        processes[m].total_ready_time = 0;
        processes[m].total_blocked_time = 0;
        processes[m].status = -1;

        enQueue(newQueue, process_id);
        pid[i] = process_id;
        i++;
        //printf("Process %3d wants to start at %6dms and run for %6dms and has %3d hard drive exchanges\n",
        	//processes[m].process_id, processes[m].start_time, processes[m].run_time, processes[m].num_exchanges);
    }

    int Timer = 0;
    int timerIncrement;
    if (processes[pid[0]%99].start_time/100 >= 1) {
    	timerIncrement = 100;
    } else if (processes[pid[0]%99].start_time/10 >= 1) {
    	timerIncrement = 10;
    } else {
    	timerIncrement = 1;
    }

    struct cpu {
		struct Process *runningProcess;
		int quantumTime;
	}cpu;
	struct hdd {
		struct Process *servicingProcess;
		int serviceTime;
	}hdd;
    cpu.runningProcess = &idleProcess;
    hdd.servicingProcess = NULL;
    hdd.serviceTime = -1;

    int p;
    int systemEmpty = isEmpty(newQueue);
    //Round Robin
    while (systemEmpty != 1) {
    	//HDD
    	if (hdd.servicingProcess == NULL) {
    		if (1 - isEmpty(blockedQueue)) {
    			p = peek(blockedQueue);
    			deQueue(blockedQueue);
    			hdd.servicingProcess = &processes[p%99];
    			processes[p].status = 0;
    			hdd.serviceTime = harddrive;
    		}
    	} else {
    		if (hdd.serviceTime == 0) {
    			enQueue(readyQueue, hdd.servicingProcess->process_id);
    			hdd.servicingProcess->status = 1;

    			if (1 - isEmpty(blockedQueue)) {
    				p = peek(blockedQueue);
    				deQueue(blockedQueue);
    				hdd.servicingProcess = &processes[p%99];
    				processes[p].status = 0;
    				hdd.serviceTime = harddrive;
    			} else {
    				hdd.servicingProcess = NULL;
    			}
    		}
    	}

    	//CPU
    	if (cpu.runningProcess == &idleProcess) {
    		if (1 - isEmpty(readyQueue)) {
    			p = peek(readyQueue);
    			deQueue(readyQueue);
    			cpu.runningProcess = &processes[p%99];
    			if (cpu.runningProcess->total_run_time == cpu.runningProcess->run_time) {
    				processes[p].status = -1;
    				cpu.runningProcess = &idleProcess;
    			} else {
    				processes[p].status = 2;
    				cpu.quantumTime = quantum;
    			}
    		}
    	} else {
    		for (int j = 0; j < cpu.runningProcess->num_exchanges; j++) {
    			if (cpu.runningProcess->exchanges[j] == cpu.runningProcess->total_run_time) {
    				if (hdd.servicingProcess == NULL) {
    					hdd.servicingProcess = cpu.runningProcess;
    					hdd.servicingProcess->status = 0;
    					hdd.serviceTime = harddrive;
    				} else {
    					enQueue(blockedQueue, cpu.runningProcess->process_id);
    					cpu.runningProcess->status = 0;
    				}

    				if (1 - isEmpty(readyQueue)) {
    					p = peek(readyQueue);
    					deQueue(readyQueue);
    					cpu.runningProcess = &processes[p%99];
    					processes[p].status = 2;
    					cpu.quantumTime = quantum;
    				} else {
    					cpu.runningProcess = &idleProcess;
    				}
    				break;
    			}
    		}

    		if (cpu.runningProcess->total_run_time == cpu.runningProcess->run_time) {
    			cpu.runningProcess->status = -1;

    			if (1 - isEmpty(readyQueue)) {
    				p = peek(readyQueue);
    				deQueue(readyQueue);
    				cpu.runningProcess = &processes[p%99];
    				processes[p].status = 2;
    				cpu.quantumTime = quantum;
    			} else {
    				cpu.runningProcess = &idleProcess;
    			}
    		}

    		if (cpu.quantumTime == 0) {
    			if (1 - isEmpty(readyQueue)) {
    				enQueue(readyQueue, cpu.runningProcess->process_id);
    				cpu.runningProcess->status = 1;

    				p = peek(readyQueue);
    				deQueue(readyQueue);
    				cpu.runningProcess = &processes[p%99];
    				processes[p].status = 2;
    				cpu.quantumTime = quantum;
    			} else {
    				cpu.quantumTime = quantum;
    			}
    		}
    	}
    	
    	//New Processes
    	int newQueueFront = peek(newQueue);
    	if ((newQueueFront != -1) && (processes[newQueueFront%99].start_time == Timer)) {
    		deQueue(newQueue);
    		if (cpu.runningProcess == &idleProcess) {
    			cpu.runningProcess = &processes[newQueueFront%99];
    			processes[newQueueFront%99].status = 2;
   				cpu.quantumTime = quantum;
    		} else {
    			enQueue(readyQueue, newQueueFront);
    			processes[newQueueFront%99].status = 1;
    		}
    	}

    	//Decerement counter for CPU and HDD
    	if (cpu.runningProcess != &idleProcess) {
    		cpu.quantumTime -= timerIncrement;
    	}
    	if (hdd.servicingProcess != NULL) {
    		hdd.serviceTime -= timerIncrement;
    	}

    	//update total times
    	for (int j = 0; j < i; j++) {
    		int t = pid[j]%99;
    		int pstat = processes[t].status;	
    		if (pstat == 0) {
    			processes[t].total_blocked_time += timerIncrement;
    		} else if (pstat == 1) {
    			processes[t].total_ready_time += timerIncrement;
    		} else if (pstat == 2) {
    			processes[t].total_run_time += timerIncrement;
    		}
    	}
    	if (cpu.runningProcess == &idleProcess) {
    		idleProcess.total_run_time += timerIncrement;
    	}

    	//increment Timer
    	Timer += timerIncrement	;
    	systemEmpty = isEmpty(newQueue) * isEmpty(readyQueue) * isEmpty(blockedQueue) *
    		(cpu.runningProcess == &idleProcess) * (hdd.servicingProcess == NULL);
    }

    //Print Results
    printf("%d %d\n", idleProcess.process_id, idleProcess.total_run_time - timerIncrement);
    for (int j = 0; j < i; j++) {
    	int m = pid[j]%99;
    	printf("%d %d %d %d\n", processes[m].process_id, processes[m].total_run_time,
    		processes[m].total_ready_time, processes[m].total_blocked_time);
    }
}