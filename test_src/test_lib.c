#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "test_lib.h"
#include "../obj_store.h"

/* implementation of utility functions common to all tests */

#define TEST_CASE_MSG_LEN 80

void ostore_option(int argc, char** argv, bool ignore_ostoreopt) {
    if (ignore_ostoreopt)
        return;
        
    if (argc > 2 && argv[2][0] == '-' 
        && (strnlen(argv[2], 2) == 2) && argv[2][1] == 'o') {
        if (enable_ostore()) {
            printf("Running test(s) with object store on ... \n");
        } else {
            printf("Cannot run with object store on ...\n");
            perror("enable_ostore() failed");
            exit(EXIT_FAILURE);
        }
    }
}

void print_test_hdr(int test_num, char *test_name)  {
    printf("---------------------------------------------------------------\n");
    printf("Running test %d: %s ...\n", test_num, test_name);
}

void print_test_ftr(int test_num, int tests_completed, struct test_defn *test) {
    printf("... test %d successfully completed %d of %d test cases\n", 
                test_num, tests_completed, test->test_cases); 
    
    if (test->mark && tests_completed == test->test_cases)
        printf("%.2f marks\n", test->mark);

    printf("---------------------------------------------------------------\n");
}

void print_usage(char* prog_name, int nr_tests, struct test_defn *test_sched, 
    bool ignore_ostoreopt) {
    printf("usage: %s -a|test_num %s\n", prog_name, 
        ignore_ostoreopt ? "" : "[-o]");
    printf("    -a to run all tests\n");
    printf("    test_num between %d and %d for a single test\n", 
                0, nr_tests - 1);
    if (!ignore_ostoreopt)
        printf("    -o 2nd argument to run tests with the object store on\n");
    printf("Test list:\n");
    for (int i = 0; i < nr_tests; i++)
        printf("%6d %s\n", i, test_sched[i].test_name);
}

void print_test_case_msg(int test_case, int line_num, int called_at, char* msg) {
    printf("[case: %03d, line: %03d", test_case, line_num);
    if (called_at)
        printf(", called at: %03d", called_at);
    printf("] %s\n", msg);
}

void run_tests(int argc, char **argv, int nr_tests,
    struct test_defn *test_sched, bool ignore_ostoreopt) {
    if (argc < 2) {
        print_usage(argv[0], nr_tests, test_sched, ignore_ostoreopt);
        return;
    }
    
    srand(time(NULL));

    if (argv[1][0] == '-' && (strnlen(argv[1], 2) == 2) && argv[1][1] == 'a') {
        // do all tests
        printf("Running tests 0 to %d (%d in total)\n", nr_tests - 1,
            nr_tests);
        ostore_option(argc, argv, ignore_ostoreopt);
        for (int i = 0; i < nr_tests; i++) {
            if (test_sched[i].test_cases) {
                print_test_hdr(i, test_sched[i].test_name);
                int tests_completed = test_sched[i].test();
                print_test_ftr(i, tests_completed, &test_sched[i]);
            }
        }
    } else {
        int tn = atoi(argv[1]);
        
        if (tn >=0 && tn < nr_tests) {
            if (test_sched[tn].test_cases) {
                ostore_option(argc, argv, ignore_ostoreopt);
                print_test_hdr(tn, test_sched[tn].test_name);
                int tests_completed = test_sched[tn].test();
                print_test_ftr(tn, tests_completed, &test_sched[tn]);
            } else {
                printf("No test cases for test %d\n", tn);
            }
        } else {
            print_usage(argv[0], nr_tests, test_sched, ignore_ostoreopt);
        }
    }
}

void assert_true(int test_case, int line_num, bool act) {
    assert_true_ca(test_case, line_num, 0, act);
}

void assert_true_ca(int test_case, int line_num, int called_at, bool act) {
    if (!act)
        print_test_case_msg(test_case, line_num, called_at,
            "actual: false, expected: true");
    
    assert(act); 
}

void assert_false(int test_case, int line_num, bool act) {
    assert_false_ca(test_case, line_num, 0, act);
}

void assert_false_ca(int test_case, int line_num, int called_at, bool act) {
    if (act)
        print_test_case_msg(test_case, line_num, called_at, 
            "actual: true, expected: false");
    
    assert(!act); 
}

void assert_eq(int test_case, int line_num, int act, int exp) {
    assert_eq_ca(test_case, line_num, 0, act, exp);
}

void assert_eq_ca(int test_case, int line_num, int called_at, int act, 
    int exp) { 
    if (act != exp) {
        char* msg = NULL;
        (void) asprintf(&msg, "actual: %d, expected: %d", act, exp);
        print_test_case_msg(test_case, line_num, called_at, msg);
        free(msg);
    }
    
    assert(act == exp); 
}

void assert_identical(int test_case, int line_num, const void* p1, 
    const void* p2) {
    assert_identical_ca(test_case, line_num, 0, p1, p2);
}

void assert_identical_ca(int test_case, int line_num, int called_at, 
    const void* p1, const void* p2) {
    if (p1 != p2)
        print_test_case_msg(test_case, line_num, called_at,
            "actual: p1 != p2, expected: identical");
    
    assert(p1 == p2); 
}

void assert_notidentical(int test_case, int line_num, const void* p1, 
    const void* p2) {
    assert_notidentical_ca(test_case, line_num, 0, p1, p2);
}

void assert_notidentical_ca(int test_case, int line_num, int called_at, 
    const void* p1, const void* p2) {
    if (p1 == p2)
        print_test_case_msg(test_case, line_num, called_at, 
            "actual: p1 == p2, expected: not identical");
    
    assert(p1 != p2); 
}

void assert_null(int test_case, int line_num, const void* p) {
    assert_null_ca(test_case, line_num, 0, p);
}

void assert_null_ca(int test_case, int line_num, int called_at, const void* p) {
    if (p)
        print_test_case_msg(test_case, line_num, called_at,
            "actual: not NULL, expected: NULL");
    
    assert(!p); 
}

void assert_notnull(int test_case, int line_num, const void* p) {
    assert_notnull_ca(test_case, line_num, 0, p);
}

void assert_notnull_ca(int test_case, int line_num, int called_at, 
    const void* p) {
    if (!p)
        print_test_case_msg(test_case, line_num, called_at,
            "actual: NULL, expected: not NULL");
    
    assert(p); 
}

