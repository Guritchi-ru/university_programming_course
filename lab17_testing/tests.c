#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <struct_and_func.h>

static void test_multiply_2x2(void **state) {
    int A[] = {1,2,3,4};
    int B[] = {5,6,7,8};
    int C[4] = {0};

    ThreadData *data = malloc(sizeof(ThreadData));
    data->nomer_str = 0;
    data->str_count = 2;
    data->n = 2;
    data->A = A;
    data->B = B;
    data->C = C;

    ProizvedMat(data);

    assert_int_equal(C[0], 1*5 + 2*7);
    assert_int_equal(C[1], 1*6 + 2*8);
    assert_int_equal(C[2], 3*5 + 4*7);
    assert_int_equal(C[3], 3*6 + 4*8);
}

static void test_multiply_3x3(void **state) {
    int A[] = {1,2,3,4,5,6,7,8,9};
    int B[] = {9,8,7,6,5,4,3,2,1};
    int C[9] = {0};

    ThreadData *data = malloc(sizeof(ThreadData));
    data->nomer_str = 0;
    data->str_count = 3;
    data->n = 3;
    data->A = A;
    data->B = B;
    data->C = C;

    ProizvedMat(data);

    assert_int_equal(C[0], 1*9 + 2*6 + 3*3);
    assert_int_equal(C[1], 1*8 + 2*5 + 3*2);
    assert_int_equal(C[2], 1*7 + 2*4 + 3*1);
    assert_int_equal(C[3], 4*9 + 5*6 + 6*3);
    assert_int_equal(C[4], 4*8 + 5*5 + 6*2);
    assert_int_equal(C[5], 4*7 + 5*4 + 6*1);
    assert_int_equal(C[6], 7*9 + 8*6 + 9*3);
    assert_int_equal(C[7], 7*8 + 8*5 + 9*2);
    assert_int_equal(C[8], 7*7 + 8*4 + 9*1);
}

static void test_filled_ones_2x2(void **state) {
    int A[] = {1,1,1,1};
    int B[] = {1,1,1,1};
    int C[4] = {0};

    ThreadData *data = malloc(sizeof(ThreadData));
    data->nomer_str = 0;
    data->str_count = 2;
    data->n = 2;
    data->A = A;
    data->B = B;
    data->C = C;

    ProizvedMat(data);

    for (int i = 0; i < 4; i++) {
        assert_int_equal(C[i], 1*1 + 1*1);
    }
}

static void test_zero_matrix(void **state) {
    int A[] = {1,2,3,4,5,6,7,8,9};
    int Zero[] = {0,0,0,0,0,0,0,0,0};
    int C[9] = {0};

    ThreadData *data = malloc(sizeof(ThreadData));
    data->nomer_str = 0;
    data->str_count = 3;
    data->n = 3;
    data->A = A;
    data->B = Zero;
    data->C = C;

    ProizvedMat(data);

    for (int i = 0; i < 9; i++) {
        assert_int_equal(C[i], 0);
    }
}

static void test_identity_3x3(void **state) {
    int A[] = {1,2,3,4,5,6,7,8,9};
    int B[] = {1,0,0,0,1,0,0,0,1};
    int C[9] = {0};

    ThreadData *data = malloc(sizeof(ThreadData));
    data->nomer_str = 0;
    data->str_count = 3;
    data->n = 3;
    data->A = A;
    data->B = B;
    data->C = C;

    ProizvedMat(data);

    for (int i = 0; i < 9; i++) {
        assert_int_equal(C[i], A[i]);
    }
}

static void test_emulate_multithread(void **sate) {
    int n = 4;
    int A[16],B[16], C[16] = {0};

    for (int i = 0; i < n*n; i++) {
        A[i] = i+1;
        B[i] = i+2;
    }

    int threads_count = 2;
    int rows_per_thread = n/threads_count;

    for (int i = 0; i < threads_count; i++) {
        ThreadData *data = malloc(sizeof(ThreadData));
        data->str_count = rows_per_thread;
        data->nomer_str = i * rows_per_thread;
        data->n = n;
        data->A = A;
        data->B = B;
        data->C = C;
        
        ProizvedMat(data);
    }

    if (n % threads_count != 0){
        ThreadData *data = malloc(sizeof(ThreadData));
        data->nomer_str = n - (n % threads_count);
        data->str_count = n % threads_count;
        data->n = n;
        data->A = A;
        data->B = B;
        data->C = C;

        ProizvedMat(data);
    }

    int non_zero = 0;
    for (int i = 0; i < n*n; i++) {
        if (C[i] != 0){
            non_zero++;
        }
    }
    assert_int_equal(non_zero, n*n);
}

static void run_multiply_2x2(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multiply_2x2),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}

static void run_multiply_3x3(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multiply_3x3),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}

static void run_filled_ones_2x2(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_filled_ones_2x2),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}

static void run_zero_matrix(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_zero_matrix),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}

static void run_identity_3x3(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_identity_3x3),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}

static void run_emulate_multithread(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_emulate_multithread),
    };
    cmocka_run_group_tests(tests, NULL, NULL);
}


int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Running all tests...\n");
        const struct CMUnitTest all_tests[] = {
            cmocka_unit_test(test_multiply_2x2),
            cmocka_unit_test(test_multiply_3x3),
            cmocka_unit_test(test_filled_ones_2x2),
            cmocka_unit_test(test_zero_matrix),
            cmocka_unit_test(test_identity_3x3),
            cmocka_unit_test(test_emulate_multithread),
        };
        return cmocka_run_group_tests(all_tests, NULL, NULL);
    }
    
    if (strcmp(argv[1], "multiply_2x2") == 0) {
        run_multiply_2x2();
    } 
    else if (strcmp(argv[1], "multiply_3x3") == 0) {
        run_multiply_3x3();
    }
    else if (strcmp(argv[1], "filled_ones_2x2") == 0) {
        run_filled_ones_2x2();
    }
    else if (strcmp(argv[1], "zero_matrix") == 0) {
        run_zero_matrix();
    }
    else if (strcmp(argv[1], "identity_3x3") == 0) {
        run_identity_3x3();
    }
    else if (strcmp(argv[1], "emulate_multithread") == 0) {
        run_emulate_multithread();
    }
    else {
        printf("Unknown test: %s\n", argv[1]);
        printf("Available tests:\n");
        printf("  multiply_2x2\n");
        printf("  multiply_3x3\n");
        printf("  filled_ones_2x2\n");
        printf("  zero_matrix\n");
        printf("  identity_3x3\n");
        printf("  emulate_multithread\n");
        return 1;
    }
    
    return 0;
}
