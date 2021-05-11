#include <stdio.h> 
#include <stdlib.h> 

struct node { 
	int key; 
	struct node* next; 
}; 

struct Queue { 
	struct node *front, *rear; 
}; 

struct node* newnode(int k) { 
	struct node* temp = (struct node*)malloc(sizeof(struct node)); 
	temp->key = k; 
	temp->next = NULL; 
	return temp; 
} 

struct Queue* createQueue() { 
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue)); 
	q->front = q->rear = NULL; 
	return q; 
} 

void enQueue(struct Queue* q, int k) { 
	struct node* temp = newnode(k); 

	if (q->rear == NULL) { 
		q->front = q->rear = temp; 
		return; 
	} 

	q->rear->next = temp; 
	q->rear = temp; 
} 

int deQueue(struct Queue* q) { 
	if (q->front == NULL) {
		return -1; 
	}

	struct node* temp = q->front; 
	free(temp); 

	q->front = q->front->next; 

	if (q->front == NULL) {
		q->rear = NULL; 
	}
	return temp->key; 
}

int peek(struct Queue* q) {
	if (q->front == NULL) {
		return -1;
	}
	return q->front->key;
}

int isEmpty(struct Queue* q) {
	if (q->front == NULL) {
		return 1;
	}
	return 0;
}