#include <stdlib.h>
#include "queue.h"

Queue *create_queue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

void enqueue(Queue *q, int client_socket) {
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    node->client_socket = client_socket;
    node->next = NULL;

    pthread_mutex_lock(&q->mutex);
    if (q->rear) {
        q->rear->next = node;
    } else {
        q->front = node;
    }
    q->rear = node;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

int dequeue(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (!q->front) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    QueueNode *node = q->front;
    int client_socket = node->client_socket;
    q->front = node->next;
    if (!q->front) {
        q->rear = NULL;
    }
    free(node);
    pthread_mutex_unlock(&q->mutex);
    return client_socket;
}

void destroy_queue(Queue *q) {
    while (q->front) {
        QueueNode *temp = q->front;
        q->front = q->front->next;
        free(temp);
    }
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    free(q);
}
