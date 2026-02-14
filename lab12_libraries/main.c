#include "compgame.h"
#include <time.h>
#include <stdlib.h> 
#include <sys/time.h>
#define N 100

int main(){
    srand(time(NULL));
    struct description *pcgames = malloc(sizeof(struct description) * N);
    struct timeval start, end;
    
    info(pcgames, N);
    printf("Исходный массив\n");
    print_games(pcgames, N);

    size_t size_of_element = sizeof(struct description);
    size_t total_size_bytes = size_of_element * N;

    printf("Сортировка по году выпуска\n");
    gettimeofday(&start, NULL);
    SelectSort(pcgames, N);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;

    print_games(pcgames, N);
    printf("\n");
    printf("ВРЕМЯ СОРТИРОВКИ:\n");
    printf("Секунды: %ld\n", seconds);
    printf("Микросекунды: %ld\n", microseconds);
    printf("РАЗМЕР ДАННЫХ\n");
    printf("Размер одной структуры: %zu байт\n", size_of_element);
    printf("Общий размер данных: %zu байт\n", total_size_bytes);

    free(pcgames);

    return 0;
}
