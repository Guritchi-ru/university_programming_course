#include "compgame.h"
#include <time.h>
#include <stdlib.h> 
#define N 5

int main(){
    srand(time(NULL));

    struct description pcgames[N];
    info(pcgames, N);
    printf("Исходный массив\n");
    print_games(pcgames, N);
    printf("Сортировка по году выпуска\n");
    SelectSort(pcgames, N);
    print_games(pcgames, N);

    return 0;
}
