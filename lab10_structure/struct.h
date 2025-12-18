#ifndef STRUCT_H
#define STRUCT_H
#include <stdio.h>

typedef struct complexnumber complex_number;
struct complexnumber{
    float x;
    float y;
};

void initComplex(complex_number* num, float real, float imag);
complex_number addComplex(complex_number a, complex_number b);
complex_number multiplyComplex(complex_number a, complex_number b);
void printComplex(complex_number num, const char* name);

#endif