struct QNode;
struct Queue;
struct QNode* newNode(int k);
struct Queue* createQueue();
void enQueue(struct Queue* q, int k);
int deQueue(struct Queue* q);
int isEmpty(struct Queue* q);
int peek(struct Queue* q);