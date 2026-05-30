#include "trainer.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[36m"

static const char *word_list[] = {"apple", "table", "chair", "window", "keyboard", "mouse", "screen", "lamp", "door", "house"};
static const char *sentences[] = {
    "The quick brown fox jumps over the lazy dog", 
    "Practice makes perfect", 
    "Typing speed is important",
    "Hello world from keyboard trainer"
};
static const char *snake_words[] = {"cat", "dog", "bird", "fish", "ant", "bee", "cow", "pig", "rat", "fox"};

static const char *dfjk_combinations[] = {
    "dddjjj",
    "fffjjj", 
    "kkkfff",
    "jdf",
    "kjdk",
    "djdk"
};
static const int dfjk_count = 6;

static const char *erui_combinations[] = {
    "eeeuuu",
    "uueue",
    "rrruuu",
    "rruru",
    "iirir",
    "iuri"
};
static const int erui_count = 6;

static const ComboLessonInfo lessons[] = {
    {"DFJK Lesson", "Practice letters: D, F, J, K", dfjk_combinations, dfjk_count},
    {"ERUI Lesson", "Practice letters: E, R, U, I", erui_combinations, erui_count}
};

static struct termios orig_termios;

static void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

static void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

static int is_valid_combo_char(char ch, ComboLesson lesson) {
    if (lesson == COMBO_LESSON_DFJK) {
        return (ch == 'd' || ch == 'f' || ch == 'j' || ch == 'k');
    } else {
        return (ch == 'e' || ch == 'r' || ch == 'u' || ch == 'i');
    }
}

static void print_colored_comparison(const char *expected, const char *user) {
    int exp_len = strlen(expected);
    int user_len = strlen(user);
    int min_len = exp_len < user_len ? exp_len : user_len;
    
    for (int i = 0; i < min_len; i++) {
        if (expected[i] == user[i]) {
            printf(COLOR_GREEN "%c" COLOR_RESET, user[i]);
        } else {
            printf(COLOR_RED "%c" COLOR_RESET, user[i]);
        }
    }
    
    if (user_len > exp_len) {
        for (int i = exp_len; i < user_len; i++) {
            printf(COLOR_RED "%c" COLOR_RESET, user[i]);
        }
    }
    
    if (exp_len > user_len) {
        for (int i = user_len; i < exp_len; i++) {
            printf(COLOR_RED "_" COLOR_RESET);
        }
    }
}

static int is_time_expired(time_t start, int duration_sec) {
    return (time(NULL) - start) >= duration_sec;
}

static int read_line_colored(char *buffer, int max_len, int expected_len, const char *expected, time_t start, int duration_sec) {
    int pos = 0;
    printf("\n> ");
    fflush(stdout);
    
    while (pos < max_len - 1) {
        if (is_time_expired(start, duration_sec)) {
            buffer[pos] = '\0';
            printf("\n");
            return pos;
        }
        
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        
        if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
            char ch = getchar();
            if (ch == '\n') {
                break;
            } else if (ch == 127 || ch == '\b') {
                if (pos > 0) {
                    pos--;
                    printf("\r> ");
                    for (int i = 0; i < pos; i++) {
                        printf(" ");
                    }
                    printf("\r> ");
                    for (int i = 0; i < pos; i++) {
                        if (i < expected_len && buffer[i] == expected[i]) {
                            printf(COLOR_GREEN "%c" COLOR_RESET, buffer[i]);
                        } else if (i < expected_len) {
                            printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                        } else {
                            printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                        }
                    }
                    fflush(stdout);
                }
            } else if (isprint(ch)) {
                buffer[pos++] = ch;
                if (pos - 1 < expected_len && ch == expected[pos - 1]) {
                    printf(COLOR_GREEN "%c" COLOR_RESET, ch);
                } else {
                    printf(COLOR_RED "%c" COLOR_RESET, ch);
                }
                fflush(stdout);
                
                if (pos == expected_len) {
                    break;
                }
            }
        }
    }
    buffer[pos] = '\0';
    printf("\n");
    return pos;
}

static int read_line_with_timeout_colored(char *buffer, int max_len, int expected_len, int timeout_sec, time_t start, int duration_sec, const char *expected) {
    int pos = 0;
    time_t word_start = time(NULL);
    printf("\n> ");
    fflush(stdout);
    
    while (time(NULL) - word_start < timeout_sec) {
        if (is_time_expired(start, duration_sec)) {
            buffer[pos] = '\0';
            printf("\n");
            return pos;
        }
        
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        
        if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
            char ch = getchar();
            if (ch == '\n') {
                break;
            } else if ((ch == 127 || ch == '\b') && pos > 0) {
                pos--;
                printf("\r> ");
                for (int i = 0; i < pos; i++) {
                    printf(" ");
                }
                printf("\r> ");
                for (int i = 0; i < pos; i++) {
                    if (i < expected_len && buffer[i] == expected[i]) {
                        printf(COLOR_GREEN "%c" COLOR_RESET, buffer[i]);
                    } else if (i < expected_len) {
                        printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                    } else {
                        printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                    }
                }
                fflush(stdout);
            } else if (isprint(ch) && pos < max_len - 1) {
                buffer[pos++] = ch;
                if (pos - 1 < expected_len && ch == expected[pos - 1]) {
                    printf(COLOR_GREEN "%c" COLOR_RESET, ch);
                } else {
                    printf(COLOR_RED "%c" COLOR_RESET, ch);
                }
                fflush(stdout);
                
                if (pos == expected_len && strncmp(buffer, expected, expected_len) == 0) {
                    break;
                }
            }
        }
    }
    buffer[pos] = '\0';
    printf("\n");
    return pos;
}

static int read_combo_line_colored(char *buffer, int max_len, int expected_len, ComboLesson lesson, time_t start, int duration_sec, const char *expected) {
    int pos = 0;
    printf("\n> ");
    fflush(stdout);
    
    while (pos < max_len - 1) {
        if (is_time_expired(start, duration_sec)) {
            buffer[pos] = '\0';
            printf("\n");
            return pos;
        }
        
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        tv.tv_sec = 0;
        tv.tv_usec = 50000;
        
        if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
            char ch = getchar();
            if (ch == '\n') {
                break;
            } else if (ch == 127 || ch == '\b') {
                if (pos > 0) {
                    pos--;
                    printf("\r> ");
                    for (int i = 0; i < pos; i++) {
                        printf(" ");
                    }
                    printf("\r> ");
                    for (int i = 0; i < pos; i++) {
                        if (i < expected_len && buffer[i] == expected[i]) {
                            printf(COLOR_GREEN "%c" COLOR_RESET, buffer[i]);
                        } else if (i < expected_len) {
                            printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                        } else {
                            printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                        }
                    }
                    fflush(stdout);
                }
            } else if (is_valid_combo_char(ch, lesson)) {
                buffer[pos++] = ch;
                if (pos - 1 < expected_len && ch == expected[pos - 1]) {
                    printf(COLOR_GREEN "%c" COLOR_RESET, ch);
                } else {
                    printf(COLOR_RED "%c" COLOR_RESET, ch);
                }
                fflush(stdout);
                
                if (pos == expected_len) {
                    break;
                }
            } else if (ch != '\n' && ch != 127 && ch != '\b') {
                printf("\n" COLOR_RED "[!] Invalid character! Use only allowed keys." COLOR_RESET "\n");
                printf("> ");
                fflush(stdout);
                while (pos > 0) {
                    pos--;
                    printf("\b \b");
                }
                for (int i = 0; i < pos; i++) {
                    if (i < expected_len && buffer[i] == expected[i]) {
                        printf(COLOR_GREEN "%c" COLOR_RESET, buffer[i]);
                    } else if (i < expected_len) {
                        printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                    } else {
                        printf(COLOR_RED "%c" COLOR_RESET, buffer[i]);
                    }
                }
                fflush(stdout);
            }
        }
    }
    buffer[pos] = '\0';
    printf("\n");
    return pos;
}

typedef struct {
    int correct;
    int errors;
} CompareResult;

static CompareResult compare_strings(const char *expected, const char *user) {
    CompareResult result = {0, 0};
    int exp_len = strlen(expected);
    int user_len = strlen(user);
    
    int min_len = exp_len < user_len ? exp_len : user_len;
    for (int i = 0; i < min_len; i++) {
        if (expected[i] == user[i]) {
            result.correct++;
        } else {
            result.errors++;
        }
    }
    
    if (user_len > exp_len) {
        result.errors += (user_len - exp_len);
    }
    if (exp_len > user_len) {
        result.errors += (exp_len - user_len);
    }
    
    return result;
}

static SessionStats run_dynamic_test(int duration_sec, const char *input_name) {
    SessionStats stats = {0, 0, 0.0, MODE_DYNAMIC};
    time_t start = time(NULL);
    int word_time_limit = 10;
    int typed_chars = 0;
    
    printf("\n" COLOR_CYAN "=== DYNAMIC MODE ===" COLOR_RESET "\n");
    printf("You have %d seconds per word!\n", word_time_limit);
    printf("Total test time: %d seconds\n", duration_sec);
    printf(COLOR_GREEN "Green" COLOR_RESET " = correct, " COLOR_RED "Red" COLOR_RESET " = error\n");
    printf("====================\n\n");
    
    enable_raw_mode();
    
    while (!is_time_expired(start, duration_sec)) {
        int idx = rand() % 10;
        const char *expected = word_list[idx];
        int expected_len = strlen(expected);
        
        time_t remaining = duration_sec - (time(NULL) - start);
        printf("\n[" COLOR_CYAN "%ld sec left" COLOR_RESET "] Type: " COLOR_CYAN "%s" COLOR_RESET " (you have %d sec)\n", 
               remaining, expected, word_time_limit);
        
        char user_input[512] = {0};
        int pos = read_line_with_timeout_colored(user_input, sizeof(user_input), expected_len, word_time_limit, start, duration_sec, expected);
        
        typed_chars += pos;
        
        int is_correct = (strcmp(user_input, expected) == 0);
        
        if (is_correct) {
            stats.correct += expected_len;
            printf(COLOR_GREEN "[✓] Correct!" COLOR_RESET "\n");
        } else {
            int exp_len = strlen(expected);
            int user_len = pos;
            int min_len = exp_len < user_len ? exp_len : user_len;
            for (int i = 0; i < min_len; i++) {
                if (expected[i] != user_input[i]) {
                    stats.errors++;
                }
            }
            if (user_len > exp_len) {
                stats.errors += (user_len - exp_len);
            }
            if (exp_len > user_len) {
                stats.errors += (exp_len - user_len);
            }
            printf(COLOR_RED "[✗] Wrong or timeout! Expected: '%s', You typed: '" COLOR_RESET, expected);
            print_colored_comparison(expected, user_input);
            printf(COLOR_RED "'" COLOR_RESET "\n");
            printf(COLOR_RED "[!] Text changed!" COLOR_RESET "\n");
        }
        
        if (is_time_expired(start, duration_sec)) {
            break;
        }
    }
    
    disable_raw_mode();
    stats.time_sec = difftime(time(NULL), start);
    
    int total_typed = typed_chars;
    double accuracy = total_typed > 0 ? (double)stats.correct / total_typed * 100 : 100;
    double speed = stats.time_sec > 0 ? stats.correct / stats.time_sec : 0;
    
    printf("\n" COLOR_CYAN "=== FINAL RESULTS ===" COLOR_RESET "\n");
    printf(COLOR_GREEN "✓ Correct symbols: %d" COLOR_RESET "\n", stats.correct);
    printf(COLOR_RED "✗ Errors: %d" COLOR_RESET "\n", stats.errors);
    printf("⌨️  Total typed: %d\n", total_typed);
    printf("📊 Accuracy: %.1f%%\n", accuracy);
    printf("⏱️  Time: %.2f sec\n", stats.time_sec);
    printf("⚡ Speed: %.2f chars/sec\n", speed);
    printf(COLOR_CYAN "====================\n" COLOR_RESET);
    
    return stats;
}

static SessionStats run_standard_test(GameMode mode, int duration_sec, const char *input_name) {
    SessionStats stats = {0, 0, 0.0, mode};
    log_message(LOG_INFO, "Starting test for %s, mode=%d, duration=%d", input_name, mode, duration_sec);
    
    time_t start = time(NULL);
    int difficulty = 1;
    int typed_chars = 0;
    
    printf("\n" COLOR_CYAN "=== START TYPING TEST ===" COLOR_RESET "\n");
    switch(mode) {
        case MODE_WORDS: printf("Mode: WORDS\n"); break;
        case MODE_SENTENCES: printf("Mode: SENTENCES\n"); break;
        case MODE_SNAKE: printf("Mode: SNAKE\n"); break;
        default: break;
    }
    printf("Duration: %d seconds\n", duration_sec);
    printf(COLOR_GREEN "Green" COLOR_RESET " = correct, " COLOR_RED "Red" COLOR_RESET " = error\n");
    printf("========================\n\n");
    
    enable_raw_mode();
    
    while (!is_time_expired(start, duration_sec)) {
        int expected_len;
        const char *expected;
        
        if (mode == MODE_WORDS) {
            int idx = rand() % 10;
            expected = word_list[idx];
            expected_len = strlen(expected);
        } else if (mode == MODE_SENTENCES) {
            int idx = rand() % 4;
            expected = sentences[idx];
            expected_len = strlen(expected);
        } else {
            static char buffer[512];
            buffer[0] = '\0';
            int num_words = (difficulty / 2) + 1;
            if (num_words > 5) num_words = 5;
            
            for (int i = 0; i < num_words; i++) {
                int idx = rand() % 10;
                strcat(buffer, snake_words[idx]);
                if (i < num_words - 1) strcat(buffer, " ");
            }
            expected = buffer;
            expected_len = strlen(expected);
        }
        
        time_t remaining = duration_sec - (time(NULL) - start);
        printf("\n[" COLOR_CYAN "%ld sec left" COLOR_RESET "] Type: " COLOR_CYAN "%s" COLOR_RESET "\n", remaining, expected);
        
        char user_input[512] = {0};
        int len = read_line_colored(user_input, sizeof(user_input), expected_len, expected, start, duration_sec);
        
        typed_chars += len;
        
        CompareResult result = compare_strings(expected, user_input);
        stats.correct += result.correct;
        stats.errors += result.errors;
        
        if (result.errors == 0 && result.correct == expected_len) {
            printf(COLOR_GREEN "[✓] Perfect!" COLOR_RESET "\n");
            if (mode == MODE_SNAKE) {
                difficulty++;
                printf(COLOR_CYAN "[SNAKE] Difficulty increased to level %d!" COLOR_RESET "\n", difficulty);
            }
        } else {
            int accuracy = expected_len > 0 ? (result.correct * 100) / expected_len : 0;
            printf(COLOR_RED "[!] Errors: %d (accuracy: %d%%)" COLOR_RESET "\n", result.errors, accuracy);
            log_message(LOG_WARNING, "User %s made error(s) in phrase: '%s' vs '%s'", 
                        input_name, expected, user_input);
        }
        
        if (is_time_expired(start, duration_sec)) {
            break;
        }
    }
    
    disable_raw_mode();
    stats.time_sec = difftime(time(NULL), start);
    
    int total_typed = typed_chars;
    double accuracy = total_typed > 0 ? (double)stats.correct / total_typed * 100 : 100;
    double speed = stats.time_sec > 0 ? stats.correct / stats.time_sec : 0;
    
    printf("\n" COLOR_CYAN "=== FINAL RESULTS ===" COLOR_RESET "\n");
    printf(COLOR_GREEN "✓ Correct symbols: %d" COLOR_RESET "\n", stats.correct);
    printf(COLOR_RED "✗ Errors: %d" COLOR_RESET "\n", stats.errors);
    printf("⌨️  Total typed: %d\n", total_typed);
    printf("📊 Accuracy: %.1f%%\n", accuracy);
    printf("⏱️  Time: %.2f sec\n", stats.time_sec);
    printf("⚡ Speed: %.2f chars/sec\n", speed);
    printf(COLOR_CYAN "====================\n" COLOR_RESET);
    
    return stats;
}

SessionStats run_combo_lesson(ComboLesson lesson, int duration_sec, const char *input_name) {
    SessionStats stats = {0, 0, 0.0, MODE_KEY_COMBOS};
    const ComboLessonInfo *lesson_info = &lessons[lesson];
    const char **combinations = lesson_info->combinations;
    int combo_count = lesson_info->combo_count;
    
    log_message(LOG_INFO, "Starting combo lesson for %s, lesson=%d, duration=%d", 
                input_name, lesson, duration_sec);
    
    time_t start = time(NULL);
    int typed_chars = 0;
    
    printf("\n" COLOR_CYAN "=== %s ===" COLOR_RESET "\n", lesson_info->name);
    printf("%s\n", lesson_info->description);
    printf("Duration: %d seconds\n", duration_sec);
    printf(COLOR_GREEN "Green" COLOR_RESET " = correct, " COLOR_RED "Red" COLOR_RESET " = error\n");
    printf("========================\n\n");
    
    enable_raw_mode();
    
    while (!is_time_expired(start, duration_sec)) {
        int idx = rand() % combo_count;
        const char *expected = combinations[idx];
        int expected_len = strlen(expected);
        
        time_t remaining = duration_sec - (time(NULL) - start);
        printf("\n[" COLOR_CYAN "%ld sec left" COLOR_RESET "] Type: " COLOR_CYAN "%s" COLOR_RESET "\n", remaining, expected);
        
        char user_input[512] = {0};
        int len = read_combo_line_colored(user_input, sizeof(user_input), expected_len, lesson, start, duration_sec, expected);
        
        typed_chars += len;
        
        CompareResult result = compare_strings(expected, user_input);
        stats.correct += result.correct;
        stats.errors += result.errors;
        
        if (result.errors == 0 && result.correct == expected_len) {
            printf(COLOR_GREEN "[✓] Perfect!" COLOR_RESET "\n");
        } else {
            int accuracy = expected_len > 0 ? (result.correct * 100) / expected_len : 0;
            printf(COLOR_RED "[!] Errors: %d (accuracy: %d%%)" COLOR_RESET "\n", result.errors, accuracy);
            log_message(LOG_WARNING, "User %s made error(s) in combo: '%s' vs '%s'", 
                        input_name, expected, user_input);
        }
        
        if (is_time_expired(start, duration_sec)) {
            break;
        }
    }
    
    disable_raw_mode();
    stats.time_sec = difftime(time(NULL), start);
    
    int total_typed = typed_chars;
    double accuracy = total_typed > 0 ? (double)stats.correct / total_typed * 100 : 100;
    double speed = stats.time_sec > 0 ? stats.correct / stats.time_sec : 0;
    
    printf("\n" COLOR_CYAN "=== FINAL RESULTS ===" COLOR_RESET "\n");
    printf(COLOR_GREEN "✓ Correct symbols: %d" COLOR_RESET "\n", stats.correct);
    printf(COLOR_RED "✗ Errors: %d" COLOR_RESET "\n", stats.errors);
    printf("⌨️  Total typed: %d\n", total_typed);
    printf("📊 Accuracy: %.1f%%\n", accuracy);
    printf("⏱️  Time: %.2f sec\n", stats.time_sec);
    printf("⚡ Speed: %.2f chars/sec\n", speed);
    printf(COLOR_CYAN "====================\n" COLOR_RESET);
    
    return stats;
}

SessionStats run_typing_test(GameMode mode, int duration_sec, const char *input_name) {
    if (mode == MODE_DYNAMIC) {
        return run_dynamic_test(duration_sec, input_name);
    }
    
    return run_standard_test(mode, duration_sec, input_name);
}