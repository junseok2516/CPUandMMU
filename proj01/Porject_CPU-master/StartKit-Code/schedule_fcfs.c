#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h" // task(name tid priorty burst)
#include "list.h" // node(node, next) insert delete traverse
#include "schedulers.h" // void add, viod schedule - needed implementation
#include "cpu.h" // void run - just invoke

struct node *haedNode = NULL;
struct node *tailNode = NULL;
int not = 0;

double avgWaitingTime(int not, int burst[], int exitT[]);
double avgTurnaroungTime(int not, int exitT[]);
double avgResponseTime(int not, int startT[]);

// add a task to the list 
void add(char *name, int priority, int burst) {
    if (haedNode == NULL) {
        haedNode = malloc(sizeof(struct node));
        tailNode = malloc(sizeof(struct node));
        haedNode->task = malloc(sizeof(struct task));
        haedNode->task->name = name;
        haedNode->task->burst = burst;
        haedNode->task->priority = priority;
        haedNode->next = NULL;
        tailNode = haedNode;
    } else {
        struct node * tmpNode = malloc(sizeof(struct node));
        tailNode->next = tmpNode;
        tmpNode->task = malloc(sizeof(struct task));
        tmpNode->task->name = name;
        tmpNode->task->burst = burst;
        tmpNode->task->priority = priority;
        tmpNode->next = NULL;    
        tailNode = tmpNode;
    }
    not++; // counts each task created
}

// invoke the scheduler
void schedule() {
    int startT[not];
    int exitT[not];
    int burst[not]; int count = 0, initialT = 0;
    for (int k = 0; k < not; k++) { // initialize all arrays
        startT[k] = 0; exitT[k] = 0; burst[k] = 0;
    }
    struct node * tmpNode = haedNode;
    while (tmpNode != NULL) {
        run(tmpNode->task, tmpNode->task->burst);
        
        burst[count] = tmpNode->task->burst;
        startT[count] = initialT;
        exitT[count] = initialT + tmpNode->task->burst;
        initialT += tmpNode->task->burst;

        count++;
        tmpNode = tmpNode->next;
    }
    printf("\nAverage waiting time = %.2f\n" , avgWaitingTime(not, burst, exitT));
    printf("Average turnaround time = %.2f\n" , avgTurnaroungTime(not, exitT));    
    printf("Average response time = %.2f\n" , avgResponseTime(not, startT));
}

double avgWaitingTime(int not, int burst[], int exitT[]) {
    float result;
    for (int i = 0; i < not; i++) {
        result += exitT[i] - burst[i];
    }
    return result/not;
}

double avgTurnaroungTime(int not, int exitT[]) {
    float result;
    for (int i = 0; i < not; i++) {
        result += exitT[i];
    }
    return result/not;
}

double avgResponseTime(int not, int startT[]) {
    float result;
    for (int i = 0; i < not; i++) {
        result += startT[i];
    }
    return result/not;
}