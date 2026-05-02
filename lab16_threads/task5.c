#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void cleanup_handler(void*arg) {
    printf("\nПоток завершается!\n");
    fflush(stdout);
}

void* printstr(void* arg) {
    char** string = (char**)arg;

    pthread_cleanup_push(cleanup_handler, string);

    for (int i = 0; string[i] != NULL; i++) {
        printf("%s ", string[i]);
        fflush(stdout);
        sleep(1);
        //printf("\n");
    }
    printf("\n");
    pthread_cleanup_pop(0);

    return NULL;
}

int main() {
    pthread_t thread[4];

    char* string1[] = {"Hello,", "world!", NULL};
    char* string2[] = {"Привет,", "мир!", NULL};
    char* string3[] = {"Bonjour", "le monde!", NULL};
    char* string4[] = {"こんにちは", "世界！", NULL};

    pthread_create(&thread[0], NULL, printstr, string1);
    pthread_create(&thread[1], NULL, printstr, string2);
    pthread_create(&thread[2], NULL, printstr, string3);
    pthread_create(&thread[3], NULL, printstr, string4);

    sleep(1);

    for (int i = 0; i < 4; i++) {
        pthread_cancel(thread[i]);
        pthread_join(thread[i], NULL);
        printf("Дочерний поток %d был прерван\n", i);
    }
}