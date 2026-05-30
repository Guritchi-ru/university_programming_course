#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/stats.h"
#include "../src/trainer.h"

static int setup(void **state) {
    (void)state;
    return 0;
}

static int teardown(void **state) {
    (void)state;
    remove("test_leaderboard.txt");
    return 0;
}

static void test_stats_creation(void **state) {
    (void)state;
    
    SessionStats s = {10, 2, 5.5, MODE_WORDS};
    
    assert_int_equal(s.correct, 10);
    assert_int_equal(s.errors, 2);
    assert_float_equal(s.time_sec, 5.5, 0.01);
    assert_int_equal(s.mode, MODE_WORDS);
}

static void test_save_new_stats(void **state) {
    (void)state;
    
    const char *test_file = "test_leaderboard.txt";
    remove(test_file);
    
    SessionStats s = {100, 5, 30.5, MODE_WORDS};
    save_stats(test_file, "TestUser", s);
    
    FILE *f = fopen(test_file, "r");
    assert_non_null(f);
    
    char name[100];
    int correct, errors, mode;
    double time;
    fscanf(f, "%99s %d %d %lf %d", name, &correct, &errors, &time, &mode);
    
    assert_string_equal(name, "TestUser");
    assert_int_equal(correct, 100);
    assert_int_equal(errors, 5);
    assert_float_equal(time, 30.5, 0.01);
    assert_int_equal(mode, MODE_WORDS);
    
    fclose(f);
}

static void test_update_better_result(void **state) {
    (void)state;
    
    const char *test_file = "test_leaderboard.txt";
    remove(test_file);
    
    SessionStats s1 = {100, 5, 30.5, MODE_WORDS};
    save_stats(test_file, "John", s1);
    
    SessionStats s2 = {150, 3, 35.0, MODE_WORDS};
    save_stats(test_file, "John", s2);
    
    FILE *f = fopen(test_file, "r");
    assert_non_null(f);
    
    char name[100];
    int correct, errors, mode;
    double time;
    fscanf(f, "%99s %d %d %lf %d", name, &correct, &errors, &time, &mode);
    
    assert_string_equal(name, "John");
    assert_int_equal(correct, 150);
    assert_int_equal(errors, 3);
    
    fclose(f);
}

static void test_keep_best_on_worse_result(void **state) {
    (void)state;
    
    const char *test_file = "test_leaderboard.txt";
    remove(test_file);
    
    SessionStats s1 = {150, 3, 35.0, MODE_WORDS};
    save_stats(test_file, "John", s1);
    
    SessionStats s2 = {100, 5, 30.5, MODE_WORDS};
    save_stats(test_file, "John", s2);
    
    FILE *f = fopen(test_file, "r");
    assert_non_null(f);
    
    char name[100];
    int correct, errors, mode;
    double time;
    fscanf(f, "%99s %d %d %lf %d", name, &correct, &errors, &time, &mode);
    
    assert_string_equal(name, "John");
    assert_int_equal(correct, 150);
    assert_int_equal(errors, 3);
    
    fclose(f);
}

static void test_different_modes_independent(void **state) {
    (void)state;
    
    const char *test_file = "test_leaderboard.txt";
    remove(test_file);
    
    SessionStats s1 = {100, 5, 30.5, MODE_WORDS};
    save_stats(test_file, "John", s1);
    
    SessionStats s2 = {200, 2, 40.0, MODE_SENTENCES};
    save_stats(test_file, "John", s2);
    
    SessionStats s3 = {50, 1, 10.0, MODE_SNAKE};
    save_stats(test_file, "John", s3);
    
    FILE *f = fopen(test_file, "r");
    assert_non_null(f);
    
    int count = 0;
    char name[100];
    int correct, errors, mode;
    double time;
    
    while (fscanf(f, "%99s %d %d %lf %d", name, &correct, &errors, &time, &mode) == 5) {
        count++;
        printf("Debug: name=%s mode=%d correct=%d\n", name, mode, correct);
    }
    
    assert_int_equal(count, 3);
    
    fclose(f);
}

static void test_multiple_users(void **state) {
    (void)state;
    
    const char *test_file = "test_leaderboard.txt";
    remove(test_file);
    
    SessionStats s1 = {100, 5, 30.5, MODE_WORDS};
    save_stats(test_file, "Alice", s1);
    
    SessionStats s2 = {150, 3, 35.0, MODE_WORDS};
    save_stats(test_file, "Bob", s2);
    
    SessionStats s3 = {120, 4, 32.0, MODE_WORDS};
    save_stats(test_file, "Alice", s3);
    
    FILE *f = fopen(test_file, "r");
    assert_non_null(f);
    
    int count = 0;
    char name[100];
    int correct, errors, mode;
    double time;
    
    while (fscanf(f, "%99s %d %d %lf %d", name, &correct, &errors, &time, &mode) == 5) {
        count++;
    }
    
    assert_int_equal(count, 2);
    
    fclose(f);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_stats_creation),
        cmocka_unit_test(test_save_new_stats),
        cmocka_unit_test(test_update_better_result),
        cmocka_unit_test(test_keep_best_on_worse_result),
        cmocka_unit_test(test_different_modes_independent),
        cmocka_unit_test(test_multiple_users),
    };
    
    return cmocka_run_group_tests(tests, setup, teardown);
}