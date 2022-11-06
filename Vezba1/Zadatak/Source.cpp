#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Node {
    int id;
    int val;
    struct Node* next;
};

struct Node* initNode(int id, int val) {
    struct Node* node = NULL;
    node = (struct Node*)malloc(sizeof(struct Node));
    node->id = id;
    node->val = val;
    node->next = NULL;
    return node;
}

void printNodes(struct Node* head) {
    while (head != NULL) {
        printf("Node %d: %d\n", head->id, head->val);
        head = head->next;
    }
}

void insertNodeAtBack(struct Node* head, struct Node* node) {
    while (head != NULL) {
        if (head->next == NULL) {
            head->next = node;
            break;
        }
        head = head->next;
    }
}

void insertNodeAtFront(struct Node** head, struct Node* node) {
    node->next = (*head);
    (*head) = node;
}

int checkIfNodeExists(struct Node* head, int id) {
    while (head != NULL) {
        if (head->id == id) {
            return 1;
        }
        head = head->next;
    }
    return -1;
}

int main() {

    struct Node* head = initNode(0, 0);

    srand((unsigned int)time(NULL));

    clock_t start_time;
    clock_t end_time;

    start_time = clock();
    for (int i = 1; i <= 100000; i++) {
        int r = rand() % 1000000 + 1;
        struct Node* node = initNode(i, r);
        insertNodeAtFront(&head, node);
    }
    end_time = clock();
    printf("Inserted at FRONT 100000 random numbers between 1 and 1000000 in %dms!\n", end_time - start_time);

    start_time = clock();
    for (int i = 1; i <= 100000; i++) {
        int r = rand() % 1000000 + 1;
        struct Node* node = initNode(i, r);
        insertNodeAtBack(head, node);
    }
    end_time = clock();
    printf("Inserted at BACK 100000 random numbers between 1 and 1000000 in %dms!\n", end_time - start_time);

    return 0;
}