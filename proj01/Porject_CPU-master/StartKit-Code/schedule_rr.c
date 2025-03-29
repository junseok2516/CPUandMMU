#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h" // task(name tid priorty burst)
#include "list.h" // node(node, next) insert delete traverse
#include "schedulers.h" // void add, viod schedule - needed implementation
#include "cpu.h" // void run - just invoke

struct node *headNode = NULL;
struct node *tailNode = NULL;
int not = 0;

double avgWaitingTime(int not, int burst[], int exitT[]);
double avgTurnaroungTime(int not, int exitT[]);
double avgResponseTime(int not, int startT[]);

// add a task to the list 
void add(char *name, int priority, int burst) {
    if (headNode == NULL) {
        headNode = malloc(sizeof(struct node));
        tailNode = malloc(sizeof(struct node));
        headNode->task = malloc(sizeof(struct task));
        headNode->task->name = name;
        headNode->task->burst = burst;
        headNode->task->priority = priority;
        headNode->next = NULL;
        tailNode = headNode;
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
    int burst[not]; int startCnt = 0, exitCnt = 0, initialT = 0;
    int q = 10, remainBurst = 0;
    for (int k = 0; k < not; k++) { // initialize all arrays
        startT[k] = 0; exitT[k] = 0; burst[k] = 0;
    }
    /* test */
    // printf("%s\n", headNode->task->name);
    // printf("%s\n", headNode->next->task->name);
    /**/
    struct node * tmpNode = headNode;
    for (int i = 0; i < not; i++) {
        burst[i] = tmpNode->task->burst;
        tmpNode = tmpNode->next;
    }
    tmpNode = headNode;
    while (headNode != NULL) {
        if (tmpNode->task->burst > q) {
            remainBurst = q;
        } else {
            remainBurst = tmpNode->task->burst;
        }
        run(tmpNode->task, remainBurst);
        tmpNode->task->burst -= remainBurst;

        if (startT[startCnt] == 0) {
            startT[startCnt++] = initialT;
        }

        if (tmpNode->task->burst <= 0) {
            exitT[exitCnt++] = initialT + remainBurst;
            delete(&headNode, tmpNode->task);
        }

        if (tmpNode->next == NULL) {
            tmpNode = headNode;
        } else {
            tmpNode = tmpNode->next;
        }
        initialT += remainBurst;
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