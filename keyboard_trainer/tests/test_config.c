#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/config_parser.h"

static const char *test_config_content = 
    "DEFAULT_TIME=45\n"
    "LOG_LEVEL=WARNING\n"
    "LEADERBOARD_FILE=test_leader.txt\n";

static void create_test_config(const char *filename) {
    FILE *f = fopen(filename, "w");
    assert_non_null(f);
    fprintf(f, "%s", test_config_content);
    fclose(f);
}

static int setup(void **state) {
    (void)state;
    return 0;
}

static int teardown(void **state) {
    (void)state;
    remove("test_config_valid.cfg");
    remove("test_config_comments.cfg");
    return 0;
}

static void test_load_valid_config(void **state) {
    (void)state;
    
    const char *test_file = "test_config_valid.cfg";
    create_test_config(test_file);
    
    Config cfg;
    int result = load_config(test_file, &cfg);
    
    assert_int_equal(result, 1);
    assert_int_equal(cfg.default_time, 45);
    assert_string_equal(cfg.log_level, "WARNING");
    assert_string_equal(cfg.leaderboard_file, "test_leader.txt");
}

static void test_load_missing_config(void **state) {
    (void)state;
    
    Config cfg;
    int result = load_config("nonexistent_file_12345.cfg", &cfg);
    
    assert_int_equal(result, 0);
    assert_int_equal(cfg.default_time, 30);
    assert_string_equal(cfg.log_level, "ERROR");
    assert_string_equal(cfg.leaderboard_file, "leaderboard.txt");
}

static void test_default_values(void **state) {
    (void)state;
    
    Config cfg;
    load_config("missing_file_that_does_not_exist.cfg", &cfg);
    
    assert_int_equal(cfg.default_time, 30);
    assert_string_equal(cfg.log_level, "ERROR");
    assert_string_equal(cfg.leaderboard_file, "leaderboard.txt");
}

static void test_config_with_comments(void **state) {
    (void)state;
    
    const char *test_file = "test_config_comments.cfg";
    FILE *f = fopen(test_file, "w");
    assert_non_null(f);
    fprintf(f, "# This is a comment\n");
    fprintf(f, "  # Another comment\n");
    fprintf(f, "DEFAULT_TIME=60\n");
    fprintf(f, "\n");
    fprintf(f, "LOG_LEVEL=ERROR\n");
    fprintf(f, "LEADERBOARD_FILE=custom.txt\n");
    fclose(f);
    
    Config cfg;
    int result = load_config(test_file, &cfg);
    
    assert_int_equal(result, 1);
    assert_int_equal(cfg.default_time, 60);
    assert_string_equal(cfg.log_level, "ERROR");
    assert_string_equal(cfg.leaderboard_file, "custom.txt");
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_load_valid_config),
        cmocka_unit_test(test_load_missing_config),
        cmocka_unit_test(test_default_values),
        cmocka_unit_test(test_config_with_comments),
    };
    
    return cmocka_run_group_tests(tests, setup, teardown);
}