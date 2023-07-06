#include <stdio.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 10
#define MAX_FILE_LINES 20
#define MAX_NUM_QUEUES 10


typedef struct {
    int id;
    int priority;
    int running;
    int duration;
} Process;

typedef struct {
    Process *processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int size;
} CircularBuffer;

typedef struct {
    Process *processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int size;
} Queue;

typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

FILE *entrada;
int quantum = 5;