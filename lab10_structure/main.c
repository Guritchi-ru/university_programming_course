#include "struct.h"

int main() {
    complex_number a, b, c, Q;
    complex_number b2, b_plus_c, temp1;

    initComplex(&a, 4.0f, 3.0f);
    initComplex(&b, 2.0f, -1.0f);
    initComplex(&c, 0.5f, 2.1f);
    
    printf("Исходные комплексные числа:\n");
    printComplex(a, "a");
    printComplex(b, "b");
    printComplex(c, "c");
    printf("\n");
    
    b2 = multiplyComplex(b, b);
    printComplex(b2, "b^2");
    b_plus_c = addComplex(b, c);
    printComplex(b_plus_c, "b + c");
    temp1 = addComplex(a, b2);
    Q = addComplex(temp1, b_plus_c);
    
    printf("\nРезультат вычисления Q = a + b^2 + (b + c):\n");
    printComplex(Q, "Q");
    
    return 0;
}