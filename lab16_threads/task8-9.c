#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int start;
    int end;
    int N;
    int* A;
    int* B;
    int* C;
} ThreadData;

void print_matrix(int N, int* matrix, const char* name) {
    printf("Матрица %s:\n", name);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%4d", matrix[i * N + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* multiply(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int N = data->N;
    int* A = data->A;
    int* B = data->B;
    int* C = data->C;

    for (int i = data->start; i < data->end; i++) {
        for (int k = 0; k < N; k++) {
            int aik = A[i * N + k];  // Загружаем один раз
            for (int j = 0; j < N; j++) {
                C[i * N + j] += aik * B[k * N + j];
            }
        }
    }

    return NULL;
}

int* allocate_matrix(int N) {
    int* matrix = (int*)malloc(N * N * sizeof(int));
    return matrix;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Использование: <имя исполняемого файла> <размер матрицы(N)> <кол-во потоков>\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int num = atoi(argv[2]);

    if (num > N) {
        num = N;
    }
    if (num <= 0) {
        printf("Кол-во потоков должно быть > 0\n");
        printf("Использование: <имя исполняемого файла> <размер матрицы(N)> <кол-во потоков>\n");
        return 1;
    }

    int* A = allocate_matrix(N);
    int* B = allocate_matrix(N);
    int* C = allocate_matrix(N);

    for (int i = 0; i < N * N; i++) {
        A[i] = 1;
        B[i] = 1;
    }

    pthread_t threads[num];
    ThreadData thread_data[num];
    int rows_for_thread = N / num;
    int remainder = N % num;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int current_row = 0;
    for (int i = 0; i < num; i++) {
        thread_data[i].N = N;
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].start = current_row;

        int rows = rows_for_thread + (i < remainder ? 1 : 0);
        thread_data[i].end = current_row + rows;
        current_row = thread_data[i].end;

        pthread_create(&threads[i], NULL, multiply, &thread_data[i]);
    }

    for (int i = 0; i < num; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed += (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;

    if (N < 5) {
        print_matrix(N, A, "A");
        print_matrix(N, B, "B");
        print_matrix(N, C, "C");
    } else {
        printf("Перемножение выполнено\n");
    }
    printf("N: %d, Потоки: %d, время: %.2f\n", N, num, elapsed);

    free(A);
    free(B);
    free(C);
}