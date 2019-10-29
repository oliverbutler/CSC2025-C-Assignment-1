#ifndef _TEST_LIB_H
#define _TEST_LIB_H
#include <stdbool.h>

/* test definition and utility functions common to all tests */

#define TEST_CASE_MSG_LEN 80

struct test_defn {
    char *test_name;
    int (*test)(void);
    int test_cases;
    double mark;
};

void print_test_case_msg(int test_case, int line_num, int called_at, char* msg);

void run_tests(int argc, char **argv, int nr_tests,
     struct test_defn *test_sched, bool ignore_ostoreopt);
      
void assert_true(int test_case, int line_num, bool act);
void assert_false(int test_case, int line_num, bool act);
void assert_eq(int test_case, int line_num, int act, int exp);
void assert_identical(int test_case, int line_num, const void* p1, 
    const void* p2);
void assert_notidentical(int test_case, int line_num, const void* p1, 
    const void* p2);
void assert_null(int test_case, int line_num, const void* p);
void assert_notnull(int test_case, int line_num, const void* p);

void assert_true_ca(int test_case, int line_num, int called_at, bool act);
void assert_false_ca(int test_case, int line_num, int called_at, bool act);
void assert_eq_ca(int test_case, int line_num, int called_at, int act, int exp);
void assert_identical_ca(int test_case, int line_num, int called_at, 
    const void* p1, const void* p2);
void assert_notidentical_ca(int test_case, int line_num, int called_at, 
    const void* p1, const void* p2);
void assert_null_ca(int test_case, int line_num, int called_at, const void* p);
void assert_notnull_ca(int test_case, int line_num, int called_at, const void* p);

#endif