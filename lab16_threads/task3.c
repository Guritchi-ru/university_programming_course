#include <stdio.h>
#include <pthread.h>

void* printstr(void* arg) {
    char** string = (char**)arg;
    for (int i = 0; string[i] != NULL; i++) {
        printf("%s ", string[i]);
    }
    printf("\n");
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

    for (int i = 0; i < 4; i++) {
        pthread_join(thread[i], NULL);
    }
}