#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct room {
    char name[50];
    int level;
    int number;
    int resolution;
    struct room *next;
};

typedef struct room n_room;

void generateRandomName(char *name) {
    char *prefixes[] = {"Dark", "Light", "Fashion", "Funny", "Colourful", 
                        "Incredible", "Beatiful", "New", "Old", "Phony"};
    char *suffixes[] = {"cave", "house", "castle", "prison", "bar", 
                        "room", "school", "university", "village", "mineshaft"};
    
    sprintf(name, "%s %s", 
            prefixes[rand() % 10], 
            suffixes[rand() % 10]);
}

n_room *newRoom(char *name, int level, int number, int resolution) {
    n_room *new_room = (n_room *)malloc(sizeof(n_room));
    if (new_room == NULL) {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }
    
    strcpy(new_room->name, name);
    new_room->level = level;
    new_room->number = number;
    new_room->resolution = resolution;
    new_room->next = NULL;
    
    return new_room;
}

void printlist(n_room *head) {
    n_room* cur = head;
    
    if (head == NULL) {
        printf("Список пуст!\n");
        return;
    }
    
    while (cur) {
        printf("  Название: %s\n", cur->name);
        printf("  Уровень сложности: %d\n", cur->level);
        printf("  Номер комнаты: %d\n", cur->number);
        printf("  Размер: %d\n", cur->resolution);
        printf("\n");
        cur = cur->next;
    }
}

n_room* appendBack(n_room* head, char *name, int level, int number, int resolution) {
    n_room* new_room = newRoom(name, level, number, resolution);
    
    if (head == NULL) {
        return new_room;
    }
    
    n_room* cur = head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    
    cur->next = new_room;
    return head;
}

void freeList(n_room *head) {
    n_room *cur = head;

    while (cur != NULL) {
        n_room *temp = cur;
        cur = cur->next;
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int n;
    n_room *head = NULL;

    printf("Введите количество комнат: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("n должно быть положительным!\n");
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        char name[50];
        generateRandomName(name);
        
        int level = rand() % 10 + 1;
        int number = i + 1;
        int resolution = rand() % 100 + 10;
        
        head = appendBack(head, name, level, number, resolution);
    }
    
    printf("\n");
    printlist(head);
    freeList(head);
    
    return 0;
}