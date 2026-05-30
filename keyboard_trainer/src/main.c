#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "trainer.h"
#include "logger.h"
#include "config_parser.h"
#include "stats.h"

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_menu() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║           KEYBOARD TRAINER             ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("====== MAIN MENU ======\n");
    printf("1. Words mode\n");
    printf("2. Sentences mode\n");
    printf("3. Snake mode (increasing difficulty)\n");
    printf("4. Dynamic mode (time limit per word)\n");
    printf("5. Key combos training\n");
    printf("6. Show leaderboard\n");
    printf("0. Exit\n");
    printf("Choice: ");
}

void print_combo_menu() {
    printf("\n===== KEY COMBOS TRAINING =====\n");
    printf("1. DFJK Lesson (d, f, j, k)\n");
    printf("2. ERUI Lesson (e, r, u, i)\n");
    printf("0. Back to main menu\n");
    printf("Choice: ");
}

int confirm_start(const char *mode_name) {
    printf("\nReady to start %s? (y/n): ", mode_name);
    char response[10];
    fgets(response, sizeof(response), stdin);
    return (response[0] == 'y' || response[0] == 'Y');
}

int main() {
    srand(time(NULL));
    
    const int MIN_TIME = 10;
    const int MAX_TIME = 600;
    
    Config cfg;
    if (!load_config("config.cfg", &cfg)) {
        fprintf(stderr, "Warning: using default config\n");
    }
    
    if (strcmp(cfg.log_level, "INFO") == 0) {
        set_log_level(LOG_INFO);
    } else if (strcmp(cfg.log_level, "WARNING") == 0) {
        set_log_level(LOG_WARNING);
    } else {
        set_log_level(LOG_ERROR);
    }
    
    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        
        if (choice >= 1 && choice <= 4) {
            const char *mode_name = "";
            GameMode mode = MODE_WORDS;
            
            switch(choice) {
                case 1: mode_name = "WORDS MODE"; mode = MODE_WORDS; break;
                case 2: mode_name = "SENTENCES MODE"; mode = MODE_SENTENCES; break;
                case 3: mode_name = "SNAKE MODE"; mode = MODE_SNAKE; break;
                case 4: mode_name = "DYNAMIC MODE"; mode = MODE_DYNAMIC; break;
            }
            
            if (!confirm_start(mode_name)) {
                printf("Test cancelled. Returning to menu.\n");
                continue;
            }
            
            int duration = cfg.default_time;
            
            if (duration < MIN_TIME) duration = MIN_TIME;
            if (duration > MAX_TIME) duration = MAX_TIME;
            
            printf("Enter test duration (seconds) [default %d, min %d, max %d]: ", duration, MIN_TIME, MAX_TIME);
            char input[32];
            fgets(input, sizeof(input), stdin);
            if (input[0] != '\n') {
                int t = atoi(input);
                if (t >= MIN_TIME && t <= MAX_TIME) {
                    duration = t;
                } else if (t > 0) {
                    printf("Invalid! Time must be between %d and %d seconds. Using default %d.\n", MIN_TIME, MAX_TIME, duration);
                } else {
                    printf("Invalid input! Using default %d.\n", duration);
                }
            }
            
            char name[64];
            printf("Enter your name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';
            if (strlen(name) == 0) strcpy(name, "Anonymous");
            
            SessionStats stats = run_typing_test(mode, duration, name);
            
            if (stats.correct > 0 || stats.errors > 0) {
                save_stats(cfg.leaderboard_file, name, stats);
            }
            
            printf("\nPress Enter to continue...");
            getchar();
            
        } else if (choice == 5) {
            int combo_choice;
            do {
                print_combo_menu();
                if (scanf("%d", &combo_choice) != 1) {
                    printf("Invalid input.\n");
                    clear_input_buffer();
                    continue;
                }
                clear_input_buffer();
                
                if (combo_choice == 0) {
                    break;
                } else if (combo_choice >= 1 && combo_choice <= 2) {
                    const char *lesson_name = (combo_choice == 1) ? "DFJK LESSON" : "ERUI LESSON";
                    ComboLesson lesson = (combo_choice == 1) ? COMBO_LESSON_DFJK : COMBO_LESSON_ERUI;
                    
                    if (!confirm_start(lesson_name)) {
                        printf("Lesson cancelled. Returning to menu.\n");
                        continue;
                    }
                    
                    int duration = cfg.default_time;
                    
                    if (duration < MIN_TIME) duration = MIN_TIME;
                    if (duration > MAX_TIME) duration = MAX_TIME;
                    
                    printf("Enter lesson duration (seconds) [default %d, min %d, max %d]: ", duration, MIN_TIME, MAX_TIME);
                    char input[32];
                    fgets(input, sizeof(input), stdin);
                    if (input[0] != '\n') {
                        int t = atoi(input);
                        if (t >= MIN_TIME && t <= MAX_TIME) {
                            duration = t;
                        } else if (t > 0) {
                            printf("Invalid! Time must be between %d and %d seconds. Using default %d.\n", MIN_TIME, MAX_TIME, duration);
                        } else {
                            printf("Invalid input! Using default %d.\n", duration);
                        }
                    }
                    
                    char name[64];
                    printf("Enter your name: ");
                    fgets(name, sizeof(name), stdin);
                    name[strcspn(name, "\n")] = '\0';
                    if (strlen(name) == 0) strcpy(name, "Anonymous");
                    
                    SessionStats stats = run_combo_lesson(lesson, duration, name);
                    
                    if (stats.correct > 0 || stats.errors > 0) {
                        save_stats(cfg.leaderboard_file, name, stats);
                    }
                    
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
                } else {
                    printf("Invalid choice. Please select 1, 2, or 0.\n");
                }
            } while (combo_choice != 0);
            
        } else if (choice == 6) {
            show_leaderboard(cfg.leaderboard_file);
        }
    } while (choice != 0);
    
    close_logger();
    return 0;
}