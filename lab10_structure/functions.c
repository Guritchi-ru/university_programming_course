#include "struct.h"

void initComplex(complex_number* num, float real, float img) {
    num->x = real;
    num->y = img;
}

complex_number addComplex(complex_number a, complex_number b) {
    complex_number result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

complex_number multiplyComplex(complex_number a, complex_number b) {
    complex_number result;
    result.x = a.x * b.x - a.y * b.y;
    result.y = a.x * b.y + a.y * b.x;
    return result;
}

void printComplex(complex_number num, const char* name) {
    if (num.y >= 0) {
        printf("%s = %.2f + %.2fi\n", name, num.x, num.y);
    } else {
        printf("%s = %.2f - %.2fi\n", name, num.x, -num.y);
    }
}