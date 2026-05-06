#ifndef COMPGAME_H
#define COMPGAME_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

struct description{
        char title[50];
        char genre[50];
        int release;
        float rating;
    };

void info(struct description pcgames[], int n);
void print_games(struct description pcgames[], int n);
void SelectSort(struct description pcgames[], int n);

#endif
