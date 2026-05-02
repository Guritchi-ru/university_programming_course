#include <stdio.h>
#include <pthread.h>

void* child_thread(void* arg) {
    for (int i = 1; i <= 5; i++){
        printf("Дочерний поток: строка %d\n", i);
    }

    return NULL;
}


int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, child_thread, NULL);
    
    pthread_join(thread, NULL);

    for (int i = 1; i <= 5; i++){
        printf("Родительский поток: строка %d\n", i);
    }

    //pthread_join(thread, NULL);
}