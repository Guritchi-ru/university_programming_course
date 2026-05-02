#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// void* sleep_and_print(void* arg) {
//     int n = *((int*)arg);
//     usleep(n*10000);
//     printf("%d ", n);
//     return NULL;
// }

// int main() {
//     int arr[] = {1, 33, 45, 6, 7, 9, 78, 2, 11, 14, 15, 19, 20, 1, 45, 78, 99, 34, 67, 99};
//     //int arr[] = {4, 1, 2, 5, 3};
//     int n = sizeof(arr) / sizeof(arr[0]);
//     pthread_t threads[n];

//     printf("Исходный массив: ");
//     for (int i = 0; i < n; i++) {
//         printf("%d ", arr[i]);
//     }
//     printf("\n");
//     printf("Отсортированный массив: ");

//     for (int i = 0; i < n; i++){
//         pthread_create(&threads[i], NULL, sleep_and_print, &arr[i]);
//     }

//     for (int i = 0; i < n; i++){
//         pthread_join(threads[i], NULL);
//     }
//     printf("\n");
// }

typedef struct {
    int value;
    int sleep;
} ThreadData;

void* sleep_and_print(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    usleep(data->sleep * 10000);
    printf("%d ", data->value);

    free(data);
    return NULL;
}

int main() {
    int arr[] = {1, 33, 45, -6, 7, 9, 78, 2, 11, 14, 15, 19, 20, 1, 45, 78, 99, 34, 67, 99};
    int n = sizeof(arr) / sizeof(arr[0]);
    pthread_t threads[n];

    int min = arr[0];
    for (int i = 1; i< n; i++) {
        if (arr[i] < min){
            min = arr[i];
        }
    }

    int offset = (min < 0) ? -min : 0;

    printf("Исходный массив: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("Отсортированный массив: ");

    for (int i = 0; i < n; i++){
        ThreadData* data = malloc(sizeof(ThreadData));
        data->value = arr[i];
        data->sleep = arr[i] + offset;

        pthread_create(&threads[i], NULL, sleep_and_print, data);
    }

    for (int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }
    printf("\n");

}
