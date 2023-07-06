#include <stdio.h>
#include <stdlib.h>

// Define os tamanhos dos Buffers
#define MAX_QUEUE_SIZE 10
#define MAX_FILE_LINES 20

int quantum = 5;

typedef struct {
    int id;
    int priority;
    int running;
    int duration;
} Processb;

typedef struct {
    Processb *processes[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int size;
} CircularBuffer;

FILE *entrada;

// Inicializa o buffer circular e abre o arquivo de entrada
void buffer_init(CircularBuffer *buffer) {
    buffer->head = 0; 
    buffer->tail = 0; 
    buffer->size = 0; 
    entrada = fopen("entrada.txt", "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        exit(1); // Encerra o programa em caso de erro
    }
}

// Remove um processo do buffer circular
Processb *buffer_dequeue(CircularBuffer *buffer) {
    if (buffer->size == 0) {
        return NULL; // Buffer vazio
    }

    Processb *process = buffer->processes[buffer->head];
    buffer->processes[buffer->head] = NULL; 
    buffer->head = (buffer->head + 1) % MAX_QUEUE_SIZE;
    buffer->size--;
    return process;
}
// Adiciona um processo no buffer circular
int buffer_enqueue(CircularBuffer *buffer) {
    if (buffer->size == MAX_QUEUE_SIZE) {
        return -1; // Buffer cheio
    }

    Processb *process = malloc(sizeof(Processb));
    if (fscanf(entrada, "%d %d %d", &(process->id), &(process->priority), &(process->duration)) != 3) {
        free(process);
        return -1; // Erro na leitura do arquivo
    }

    process->running = 0; // Inicializa process.running como 0

    // Encontra a posição correta para inserir o processo na fila com base na prioridade
    int insert_index = buffer->head;
    while (buffer->processes[insert_index] != NULL &&
           buffer->processes[insert_index]->priority >= process->priority) {
        insert_index = (insert_index + 1) % MAX_QUEUE_SIZE;
    }

    // Desloca os processos para abrir espaço para o novo processo
    int current_index = buffer->tail;
    while (current_index != insert_index) {
        int prev_index = (current_index - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
        buffer->processes[current_index] = buffer->processes[prev_index];
        current_index = prev_index;
    }

    // Insere o novo processo na posição correta
    buffer->processes[insert_index] = process;
    buffer->tail = (buffer->tail + 1) % MAX_QUEUE_SIZE;
    buffer->size++;

    return 0;
}

// Imprime as informações de um processo na tela e registra em um arquivo de saída
void print_processb(Processb *process) {
    int cont = process->duration;
    while (cont > 0)
    {
        printf("Processb ID: %d -- Priority: %d -- Duration: %d", process->id, process->priority, cont);
        printf("\n");
        cont = cont - quantum;

        
    FILE *arquivo = fopen("saidaBatch.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    fprintf(arquivo, "Process ID: %d -- Priority: %d -- Duration: %d\n", process->id, process->priority, process->duration);

    fclose(arquivo);
    }
}

// Executa o lote de processos
void batch(){
    CircularBuffer buffer;
    buffer_init(&buffer);

    // Colocar os primeiros 10 processos no buffer circular
    int i;
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        int result = buffer_enqueue(&buffer);
        if (result == -1) {
            printf("Erro ao adicionar processo ao buffer.\n");
            break;
        }
    }

    // Desenfileirar e imprimir todos os processos do buffer circular
    Processb *process;
    FILE *arquivo = fopen("saidaBatch.txt", "a");
    while ((process = buffer_dequeue(&buffer)) != NULL) {
        print_processb(process);
        free(process);
    }

    // Ler e armazenar os próximos 10 processos no buffer circular
    for (i = 0; i < MAX_FILE_LINES - MAX_QUEUE_SIZE; i++) {
        int result = buffer_enqueue(&buffer);
        if (result == -1) {
            printf("Erro ao adicionar processo ao buffer.\n");
            break;
        }
    }

    // Desenfileirar e imprimir os processos restantes do buffer circular
    while ((process = buffer_dequeue(&buffer)) != NULL) {
        print_processb(process);
        free(process);
    }

    fclose(entrada);

}

