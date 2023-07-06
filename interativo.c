#include <stdio.h>
#include <stdlib.h>



// Define os tamanhos dos Buffers
#define MAX_QUEUE_SIZE 10
#define MAX_NUM_QUEUES 10

// Define a estrutura para armazenar informações de um processo

typedef struct {
    int id;
    int priority;
    int running;
    int duration;
} Process;

// Define a estrutura de uma fila de prioridade
typedef struct {
    Process *processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int size;
} Queue;

// Define a estrutura do escalonador
typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;

// Inicializa a Fila
void queue_init(Queue *q) {
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

// Adiciona um processo no buffer circular
int enqueue(Queue *q, Process *process) {
    if (q->size == MAX_QUEUE_SIZE) {
        return -1; // Fila cheia
    }

    q->processes[q->tail] = process;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    q->size++;
    return 0;
}

// Remove um processo do Buffer circular
Process *dequeue(Queue *q) {
    if (q->size == 0) {
        return NULL; // Fila vazia
    }

    Process *process = q->processes[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return process;
}

// Inicializa o número total de filas de prioridade
void scheduler_init(Scheduler *scheduler) {
    scheduler->num_queues = 0;
}

// Adicionar um processo à fila de prioridade especificada
int scheduler_enqueue(Scheduler *scheduler, int queue_index, Process *process) {
    if (queue_index < 0 || queue_index >= scheduler->num_queues) {
        return -1; // Índice de fila inválido
    }

    Queue *queue = &(scheduler->queues[queue_index]);
    return enqueue(queue, process);
}

//Desenfileirar o processo de maior prioridade dentre todas as filas de prioridade no escalonador 
Process *scheduler_dequeue_highest_priority(Scheduler *scheduler) {
    Process *highest_priority_process = NULL;
    int i = 0;
    int empty_queues = 0;

    while (empty_queues < scheduler->num_queues) {
        Queue *queue = &(scheduler->queues[i]);

        if (queue->size > 0) {
            Process *process = queue->processes[queue->head];

            if (highest_priority_process == NULL || process->priority < highest_priority_process->priority) {
                highest_priority_process = process;
            }
        } else {
            empty_queues++;
        }

        i = (i + 1) % scheduler->num_queues;
    }

    if (highest_priority_process != NULL) {
        Queue *queue = &(scheduler->queues[highest_priority_process->priority]);
        return dequeue(queue);
    }

    return NULL;
}

// Imprime as informações de um processo na tela e as registra no arquivo
void print_process(Process *process) {
    printf("Process ID: %d -- Priority: %d -- Duration: %d", process->id, process->priority, process->duration);
    printf("\n");

    FILE *arquivo = fopen("saidaInterative.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    fprintf(arquivo, "Process ID: %d -- Priority: %d -- Duration: %d\n", process->id, process->priority, process->duration);

    fclose(arquivo);
}

void interactive() {
    Scheduler scheduler;
    scheduler_init(&scheduler);

    int num_queues = 3; // Número fixo de filas de prioridade
    scheduler.num_queues = num_queues;

    // Inicializa as filas
    int i;
    for (i = 0; i < num_queues; i++) {
        queue_init(&(scheduler.queues[i]));
    }

    // Entrada dos processos através do arquivo "entrada.txt"
    FILE *entrada = fopen("entrada.txt", "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    int process_id = 1;
    int queue_index, priority, duration;

    while (fscanf(entrada, "%d %d %d", &queue_index, &priority, &duration) == 3) {
        

        if (priority < 0 || duration <= 0) {
            printf("Dados do processo inválidos. Ignorando processo.\n");
            continue;
        }

        Process *process = malloc(sizeof(Process));
        process->id = process_id++;
        process->priority = priority;
        process->running = 0;
        process->duration = duration;

        // Verifica a faixa de prioridade e adiciona o processo à fila correspondente
        if (priority >= 0 && priority <= 10) {
            int result = scheduler_enqueue(&scheduler, 0, process); // Fila 1
            if (result == -1) {
                printf("Erro ao adicionar processo à fila 1 de prioridade. Fila cheia.\n");
                free(process);
            }
        } else if (priority >= 11 && priority <= 20) {
            int result = scheduler_enqueue(&scheduler, 1, process); // Fila 2
            if (result == -1) {
                printf("Erro ao adicionar processo à fila 2 de prioridade. Fila cheia.\n");
                free(process);
            }
        } else if (priority >= 21 && priority <= 30) {
            int result = scheduler_enqueue(&scheduler, 2, process); // Fila 3
            if (result == -1) {
                printf("Erro ao adicionar processo à fila 3 de prioridade. Fila cheia.\n");
                free(process);
            }
        } else {
            printf("Dados do processo inválidos. Prioridade fora do intervalo esperado. Ignorando processo.\n");
            free(process);
        }
    }

    fclose(entrada);

    // Execução dos processos
    int current_queue_index = 0;
    int all_queues_empty = 0;
    int quantum = 5;
    FILE *arquivo = fopen("saidaInterative.txt", "w");
    while (!all_queues_empty) {
        Queue *current_queue = &(scheduler.queues[current_queue_index]);

        if (current_queue->size > 0) {
            int tick_count = 0;
            
            while (tick_count < quantum && current_queue->size > 0) {
                Process *process = dequeue(current_queue);
                process->running = 1;
                print_process(process);
                process->duration = process->duration - quantum;

                if (process->duration > 0) {
                    int result = enqueue(current_queue, process);
                    if (result == -1) {
                        printf("Erro ao adicionar processo à fila de prioridade. Fila cheia.\n");
                        free(process);
                    }
                } else {
                    free(process);
                }

                tick_count++;
            }
        }

        // Verifica se todas as filas estão vazias
        all_queues_empty = 1;
        for (i = 0; i < num_queues; i++) {
            if (scheduler.queues[i].size > 0) {
                all_queues_empty = 0;
                break;
            }
        }

        // Passa para a próxima fila
        current_queue_index = (current_queue_index + 1) % num_queues;
    }
}
