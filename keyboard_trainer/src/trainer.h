#ifndef TRAINER_H
#define TRAINER_H

#include "stats.h"

typedef enum { 
    MODE_WORDS, 
    MODE_SENTENCES, 
    MODE_SNAKE, 
    MODE_DYNAMIC,
    MODE_KEY_COMBOS 
} GameMode;

typedef enum {
    COMBO_LESSON_DFJK,
    COMBO_LESSON_ERUI
} ComboLesson;

typedef struct {
    const char *name;
    const char *description;
    const char **combinations;
    int combo_count;
} ComboLessonInfo;

SessionStats run_typing_test(GameMode mode, int duration_sec, const char *input_name);
SessionStats run_combo_lesson(ComboLesson lesson, int duration_sec, const char *input_name);

#endif