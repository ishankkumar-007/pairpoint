#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

// Task Queue structures
typedef struct QueueNode {
    int client_socket;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

// Queue functions
Queue *create_queue(void);
void enqueue(Queue *q, int client_socket);
int dequeue(Queue *q);
void destroy_queue(Queue *q);

#endif // QUEUE_H
