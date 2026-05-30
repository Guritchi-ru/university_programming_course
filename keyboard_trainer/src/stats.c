#include "stats.h"
#include "logger.h"
#include "trainer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char name[100];
    int correct;
    int errors;
    double time;
    int mode;
} LeaderboardEntry;

static const char* mode_to_string(int mode) {
    switch(mode) {
        case MODE_WORDS: return "WORDS";
        case MODE_SENTENCES: return "SENTENCES";
        case MODE_SNAKE: return "SNAKE";
        case MODE_DYNAMIC: return "DYNAMIC";
        case MODE_KEY_COMBOS: return "COMBOS";
        default: return "UNKNOWN";
    }
}

static int compare_entries(const void *a, const void *b) {
    const LeaderboardEntry *ea = (const LeaderboardEntry*)a;
    const LeaderboardEntry *eb = (const LeaderboardEntry*)b;
    
    if (ea->mode != eb->mode) {
        return ea->mode - eb->mode;
    }
    
    double accuracy_a = (double)ea->correct / (ea->correct + ea->errors) * 100;
    double accuracy_b = (double)eb->correct / (eb->correct + eb->errors) * 100;
    
    if (accuracy_a != accuracy_b) {
        return (accuracy_b > accuracy_a) ? 1 : -1;
    }
    
    double speed_a = ea->correct / ea->time;
    double speed_b = eb->correct / eb->time;
    if (speed_a != speed_b) {
        return (speed_b > speed_a) ? 1 : -1;
    }
    
    if (ea->correct != eb->correct) {
        return eb->correct - ea->correct;
    }
    
    return 0;
}

static int is_better_result(int new_correct, int new_errors, double new_time, 
                            int old_correct, int old_errors, double old_time) {
    if (new_correct != old_correct) {
        return new_correct > old_correct;
    }
    
    if (new_errors != old_errors) {
        return new_errors < old_errors;
    }
    
    double new_speed = new_correct / new_time;
    double old_speed = old_correct / old_time;
    return new_speed > old_speed;
}

void save_stats(const char *filename, const char *name, SessionStats s) {
    FILE *f = fopen(filename, "r");
    LeaderboardEntry entries[100];
    int count = 0;
    int updated = 0;
    
    if (f) {
        while (count < 100 && fscanf(f, "%99s %d %d %lf %d", 
               entries[count].name, &entries[count].correct, 
               &entries[count].errors, &entries[count].time, 
               &entries[count].mode) == 5) {
            count++;
        }
        fclose(f);
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].name, name) == 0 && entries[i].mode == s.mode) {
            if (is_better_result(s.correct, s.errors, s.time_sec,
                                 entries[i].correct, entries[i].errors, entries[i].time)) {
                entries[i].correct = s.correct;
                entries[i].errors = s.errors;
                entries[i].time = s.time_sec;
                updated = 1;
                log_message(LOG_INFO, "Updated stats for %s in mode %d: new correct=%d (was %d)", 
                            name, s.mode, s.correct, entries[i].correct);
            } else {
                log_message(LOG_INFO, "Stats for %s in mode %d not updated (current result is worse)", 
                            name, s.mode);
                updated = 1;
            }
            break;
        }
    }
    
    if (!updated && count < 100) {
        strcpy(entries[count].name, name);
        entries[count].correct = s.correct;
        entries[count].errors = s.errors;
        entries[count].time = s.time_sec;
        entries[count].mode = s.mode;
        count++;
        log_message(LOG_INFO, "Added new stats for %s in mode %d: correct=%d", 
                    name, s.mode, s.correct);
    }
    
    qsort(entries, count, sizeof(LeaderboardEntry), compare_entries);
    
    f = fopen(filename, "w");
    if (!f) {
        log_message(LOG_ERROR, "Cannot open leaderboard file %s for writing", filename);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s %d %d %.2f %d\n", 
                entries[i].name, entries[i].correct, 
                entries[i].errors, entries[i].time, entries[i].mode);
    }
    fclose(f);
}

void show_leaderboard(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("\n--- Leaderboard empty ---\n");
        return;
    }
    
    LeaderboardEntry entries[100];
    int count = 0;
    
    while (count < 100 && fscanf(f, "%99s %d %d %lf %d", 
           entries[count].name, &entries[count].correct, 
           &entries[count].errors, &entries[count].time, 
           &entries[count].mode) == 5) {
        count++;
    }
    fclose(f);
    
    if (count == 0) {
        printf("\n--- Leaderboard empty ---\n");
        return;
    }
    
    qsort(entries, count, sizeof(LeaderboardEntry), compare_entries);
    
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║              LEADERBOARD                 ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("===================================================================================================\n");
    
    int current_mode = -1;
    for (int i = 0; i < count; i++) {
        if (entries[i].mode != current_mode) {
            current_mode = entries[i].mode;
            printf("\n--- %s MODE ---\n", mode_to_string(current_mode));
        }
        
        double speed = entries[i].correct / entries[i].time;
        double accuracy = (double)entries[i].correct / (entries[i].correct + entries[i].errors) * 100;
        
        printf("  %-15s | correct: %4d | errors: %3d | speed: %5.2f ch/s | time: %5.2f sec | acc: %5.1f%%\n",
               entries[i].name, entries[i].correct, entries[i].errors, speed, entries[i].time, accuracy);
    }
    printf("===================================================================================================\n");
}
