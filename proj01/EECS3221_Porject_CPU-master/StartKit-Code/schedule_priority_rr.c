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
struct node * getPriority(struct node * node);
struct node * getrrPriority(struct node * node, struct node * currNode);
int cntTheSmaePriorities(struct node * node);
void addToTail(struct node **head, Task *task);
int extractTaskNum(Task * task);

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
    int startT[not]; int checkArr[not];
    int exitT[not];
    int burst[not]; int exitCnt = 0, initialT = 0;
    int q = 10, remainBurst = 0;
    for (int k = 0; k < not; k++) { // initialize all arrays
        startT[k] = 0; exitT[k] = 0; burst[k] = 0, checkArr[not] = 0;
    }
    struct node * moveNode = NULL;
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
    moveNode = getPriority(headNode);
    while (headNode != NULL) {
        if (moveNode->task->burst > q && cntTheSmaePriorities(headNode) > 1) {
            remainBurst = q;
        } else {
            remainBurst = moveNode->task->burst;
        }
        run(moveNode->task, remainBurst);
        moveNode->task->burst -= remainBurst;
        if (cntTheSmaePriorities(headNode) > 1 && moveNode->task->burst > 0) {
            delete(&headNode, moveNode->task);
            addToTail(&headNode, moveNode->task);
        } 

        if (checkArr[extractTaskNum(moveNode->task)] == 0) {
            startT[extractTaskNum(moveNode->task)] = initialT;
            checkArr[extractTaskNum(moveNode->task)] = 1;
        }

        if (moveNode->task->burst <= 0) {
            exitT[exitCnt++] = initialT + remainBurst;
            delete(&headNode, moveNode->task);
            moveNode = getPriority(headNode);
        } else {
            moveNode = getrrPriority(headNode, moveNode);
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

struct node * getPriority(struct node * node) {
    struct node *tmp = node;
    struct node *currPri = node;
    while(tmp != NULL) {
        if (tmp->task->priority > currPri->task->priority) {
            currPri = tmp;
        }
        tmp = tmp->next;
    }
    return currPri;
}

struct node * getrrPriority(struct node * node, struct node * currNode) {
    struct node *tmp = node;
    struct node *currPri = currNode; 
    struct node *movePri = currNode;
    while (tmp != NULL) {
        if (currPri->task->priority <= tmp->task->priority) {
            if (strcmp(tmp->task->name, movePri->task->name) != 0) {
                return tmp;
            }
        }
        tmp = tmp->next;
    }
    return currPri;
}

int cntTheSmaePriorities(struct node * node) {
    struct node *tmp = node;
    struct node *currPri = getPriority(node);
    int cnt = 0;
    while (tmp != NULL) {
        if (tmp->task->priority == currPri->task->priority) {
            cnt++;
        }
        tmp = tmp->next;
    }
    return cnt;
}

void addToTail(struct node ** node, Task *currTask){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->task = currTask;
    newNode->next = NULL;
    if ((*node) == NULL){
        *node = newNode;
    } else {
        struct node *tmp = *node;
        while (tmp->next != NULL){
            tmp = tmp->next;
        }
        tmp->next = newNode;
    }
}

int extractTaskNum(Task * task) {
    return atoi(&task->name[1]);
}