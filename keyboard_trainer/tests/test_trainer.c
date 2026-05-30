#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "../src/trainer.h"
#include "../src/stats.h"

static int setup_logger(void **state) {
    (void)state;
    return 0;
}

static int teardown_logger(void **state) {
    (void)state;
    return 0;
}

static void test_modes_initialization(void **state) {
    (void)state;
    
    assert_int_not_equal(MODE_WORDS, MODE_SENTENCES);
    assert_int_not_equal(MODE_SENTENCES, MODE_SNAKE);
    assert_int_not_equal(MODE_SNAKE, MODE_DYNAMIC);
    assert_int_not_equal(MODE_DYNAMIC, MODE_KEY_COMBOS);
    
    assert_true(MODE_WORDS >= 0 && MODE_WORDS <= 4);
    assert_true(MODE_SENTENCES >= 0 && MODE_SENTENCES <= 4);
    assert_true(MODE_SNAKE >= 0 && MODE_SNAKE <= 4);
    assert_true(MODE_DYNAMIC >= 0 && MODE_DYNAMIC <= 4);
    assert_true(MODE_KEY_COMBOS >= 0 && MODE_KEY_COMBOS <= 4);
}

static void test_stats_calculation(void **state) {
    (void)state;
    
    SessionStats stats = {50, 3, 10.0, MODE_WORDS};
    
    double speed = stats.correct / stats.time_sec;
    assert_float_equal(speed, 5.0, 0.01);
    
    double accuracy = (double)stats.correct / (stats.correct + stats.errors) * 100;
    assert_float_equal(accuracy, 94.34, 0.01);
}

static void test_combo_lesson_enum(void **state) {
    (void)state;
    
    assert_int_equal(COMBO_LESSON_DFJK, 0);
    assert_int_equal(COMBO_LESSON_ERUI, 1);
}

static void test_session_stats_with_mode(void **state) {
    (void)state;
    
    SessionStats stats = {0, 0, 0.0, MODE_KEY_COMBOS};
    
    assert_int_equal(stats.correct, 0);
    assert_int_equal(stats.errors, 0);
    assert_float_equal(stats.time_sec, 0.0, 0.01);
    assert_int_equal(stats.mode, MODE_KEY_COMBOS);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_modes_initialization),
        cmocka_unit_test(test_stats_calculation),
        cmocka_unit_test(test_combo_lesson_enum),
        cmocka_unit_test(test_session_stats_with_mode),
    };
    
    return cmocka_run_group_tests(tests, setup_logger, teardown_logger);
}