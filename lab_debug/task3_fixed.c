#include <stdio.h>

//ошибка приоритета операторов в макросе из-за отсутствия скобок

#define SQR(x) ((x) * (x))

int main() {
    int y = 5;
    int z = SQR(y + 1);
    printf("z = %d\n", z);
    return 0;
}