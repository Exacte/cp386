/*
DScheduler.c

Student Name : Mason Cooper
Student ID # : 140328200

*/

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dscheduler.h"


/*
Any required standard libraries and your header files here
*/

struct schedulerInput loadRequest(){
    struct schedulerInput results;
    int numRequests;
    char line_buffer[MAX_LINE_LENGTH];
    char direction;
    char *token;
    
    //Process simulation input line by line
    fgets(line_buffer, MAX_LINE_LENGTH, stdin);

    token = strtok(line_buffer, " ");
    sscanf(token, "%d",&numRequests);
    
    token = strtok(NULL, " ");
    sscanf(token, "%d",&results.startTrack);
       
    token = strtok(NULL, " ");
    sscanf(token, "%c",&direction);
    results.direction = direction == 'u' ? 1 : -1;
    
    results.requests.elements = numRequests;
    results.requests.data = (int *)malloc(sizeof(int)*numRequests);
    if ( results.requests.data == NULL ){
        fprintf( stderr, "Was unable to allocate space for requests data.\n");
        exit( BAD_MALLOC );
    }
    
    for (int i = 0; i < numRequests; i++){
        token = strtok(NULL, " ");
        sscanf(token, "%d", &results.requests.data[i]);
    }
    
    return results;
}

void printResults(struct schedulerResult results){
    for (int i = 0; i < results.requests.elements; i++){
        printf("%4d", results.requests.data[i]);
    }
    printf(" Total Head Movement %5d\n", results.totalHeadMovement);
}

struct schedulerResult processRequest(enum POLICIES policy, struct schedulerInput request){
    struct schedulerResult results;
    results.totalHeadMovement = 0;
    
    switch(policy){
        case FCFS:
            return process_FCFS_request(request);
        case SSTF:
            return process_SSTF_request(request);
        case SCAN:
            return process_SCAN_request(request);
        case C_SCAN:
            return process_C_SCAN_request(request);
    }
    return results;
}



/* Fill in the following functions */
struct schedulerResult process_FCFS_request(struct schedulerInput request){
    struct schedulerResult results;
    results.totalHeadMovement = 0;
    results.requests.elements = request.requests.elements;
    results.requests.data  = (int *)malloc(sizeof(int)*request.requests.elements);

    results.requests.data[0] = request.requests.data[0];
    results.totalHeadMovement += abs(request.startTrack - request.requests.data[0]);

    for (int i = 1; i < results.requests.elements; i++) {
    	results.requests.data[i] = request.requests.data[i];
    	results.totalHeadMovement += abs(results.requests.data[i-1] - request.requests.data[i]);
    }
    return results;
}

struct schedulerResult process_SSTF_request(struct schedulerInput request){
    struct schedulerResult results;
    results.totalHeadMovement = 0;
    results.requests.elements = request.requests.elements;
    results.requests.data  = (int *)malloc(sizeof(int)*request.requests.elements);

    int completed = 0;
    int headLocation = request.startTrack;
    int ST;
    int STL = 0;

    while (completed != results.requests.elements) {
    	ST = 10000;
    	for (int i = 0; i < results.requests.elements; i++) {
    		if (request.requests.data[i] != -1) {
    			int temp = abs(headLocation - request.requests.data[i]);
	    		if (temp < ST) {
	    			ST = temp;
	    			STL = i;
	    		}
    		}
    	}

    	headLocation = request.requests.data[STL];
    	results.requests.data[completed] = request.requests.data[STL];
    	request.requests.data[STL] = -1;
    	results.totalHeadMovement += ST;

    	completed += 1;
    }


    return results;
}

struct schedulerResult process_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
    results.totalHeadMovement = 0;
    results.requests.elements = request.requests.elements;
    results.requests.data  = (int *)malloc(sizeof(int)*request.requests.elements);

    int completed = 0;
    int headLocation = request.startTrack;
    int adder = request.direction;
    int edge;
    if (adder > 0) {
    	edge = 1023;
    } else {
    	edge = 0;
    }

    while (completed != results.requests.elements) {
    	if (headLocation == edge) adder = adder*-1;

    	for (int i = 0; i < results.requests.elements; i++) {
    		if (request.requests.data[i] != -1 && request.requests.data[i] == headLocation) {
    			results.requests.data[completed] = headLocation;
    			request.requests.data[i] = -1;
    			completed += 1;
    		}
    	}

    	headLocation += adder;
    	results.totalHeadMovement += 1;
    }
    results.totalHeadMovement -= 1;

    return results;
}

struct schedulerResult process_C_SCAN_request(struct schedulerInput request){
    struct schedulerResult results;
    results.totalHeadMovement = 0;
    results.requests.elements = request.requests.elements;
    results.requests.data  = (int *)malloc(sizeof(int)*request.requests.elements);

    int completed = 0;
    int headLocation = request.startTrack;
    int edge = 1023;
    if (request.direction < 0) {
    	results.totalHeadMovement += request.startTrack;
    	headLocation = 0;
    }

    while (completed != results.requests.elements) {
    	if (headLocation == edge) {
    		results.totalHeadMovement += 1023;
    		headLocation = 0;
    	}

    	for (int i = 0; i < results.requests.elements; i++) {
    		if (request.requests.data[i] != -1 && request.requests.data[i] == headLocation) {
    			results.requests.data[completed] = headLocation;
    			request.requests.data[i] = -1;
    			completed += 1;
    		}
    	}

    	headLocation += 1;
    	results.totalHeadMovement += 1;
    }
    results.totalHeadMovement -= 1;

    return results;
}
