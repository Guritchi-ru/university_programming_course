#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int turn = 0;

void* printstrchild(void* arg) {
    char** string = (char**)arg;
    for (int i = 0; string[i] != NULL; i++) {
        pthread_mutex_lock(&mutex);
        while (turn != 1)
            pthread_cond_wait(&cond, & mutex);

        printf("Дочерний поток: %s\n", string[i]);
        turn = 0; 
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t thread;

    char* stringparent[] = {"Bonjour", "le monde!", NULL};
    char* stringchild[] = {"こんにちは", "世界！", NULL};

    pthread_create(&thread, NULL, printstrchild, stringchild);

    for (int i = 0; stringparent[i] != NULL; i++) {
        pthread_mutex_lock(&mutex);
        while (turn != 0)
            pthread_cond_wait(&cond, &mutex);

        printf("Родительский поток: %s\n", stringparent[i]);
        turn = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    pthread_join(thread, NULL);
}