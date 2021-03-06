#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
typedef struct {
  int P_time;
  int P_id;
  char P_status;
  int P_service_time;
  int P_ex_time[4];
} Process;

typedef struct node {
    Process process;
    struct node *next;
} node_t;

void input_process(){
  int i, j, pnum, size;
  int *v;
  node_t *head = NULL;
  
  printf("Enter number of processes: ");
  scanf("%d", &pnum);
  Process process[pnum];

  printf("Enter amount of exhanges with hard drive: ");
  scanf("%d", &size);
  v = malloc(size * sizeof(int));
  
  for(i=0; i<pnum; i++){
    for(j=0; j<size; j++)
      scanf("%d", &v[j]);
    
    process[i].P_time = v[0];
    process[i].P_id = v[1];
    process[i].P_service_time = v[2];
    for(j=3; j<size; j++)
      process[i].P_ex_time[j] = v[j];
    enqueue(&head, process[i]);
  }
}

void enqueue(node_t **head, Process process) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->process = process;
    new_node->next = *head;

    *head = new_node;
}

Process dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    Process retprocess;

    if (*head == NULL) return retprocess;

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retprocess = current->process;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retprocess;
}

void print_list(node_t *head) {
    node_t *current = head;

    while (current != NULL) {
        printf("%d\n", current->process.P_id);
        current = current->next;
    }
}

int main() {
    input_process();
    
}
