#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "test_lib.h"
#include "strtest_lib.h"
#include "../string_o.h"

#define NR_TESTS 16

/* test functions */
int test_newString();
int test_deleteString();
int test_char_at_norm();
int test_char_at_err();
int test_concat_norm();
int test_concat_err();
int test_equals();
int test_get_value_norm();
int test_get_value_err();
int test_index_of_norm();
int test_index_of_err();
int test_length();
int test_split_norm();
int test_split_err();
int test_substring_norm();
int test_substring_err();

struct test_defn test_schedule[NR_TESTS] = {
    { "test_newString",         test_newString,         19, 0 },   /* test  0 */
    { "test_deleteString",      test_deleteString,       4, 0 },   /* test  1 */
    { "test_char_at_norm",      test_char_at_norm,      12, 0 },   /* test  2 */
    { "test_char_at_err",       test_char_at_err,        8, 0 },   /* test  3 */
    { "test_concat_norm",       test_concat_norm,       81, 0 },   /* test  4 */
    { "test_concat_err",        test_concat_err,         8, 0 },   /* test  5 */
    { "test_equals",            test_equals,            59, 0 },   /* test  6 */
    { "test_get_value_norm",    test_get_value_norm,    92, 0 },   /* test  7 */
    { "test_get_value_err",     test_get_value_err,      8, 0 },   /* test  8 */
    { "test_index_of_norm",     test_index_of_norm,      8, 0 },   /* test  9 */
    { "test_index_of_err",      test_index_of_err,       8, 0 },   /* test 10 */
    { "test_length",            test_length,            14, 0 },   /* test 11 */
    { "test_split_norm",        test_split_norm,       170, 0 },   /* test 12 */
    { "test_split_err",         test_split_err,          6, 0 },   /* test 13 */
    { "test_substring_norm",    test_substring_norm,   112, 0 },   /* test 14 */
    { "test_substring_err",     test_substring_err,     24, 0 },   /* test 15 */
};

int main(int argc, char** argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, false);
    
    return 0;
}

/* test helper functions */
void assert_concat_success(int test_case, int called_at, String lhs, String rhs, 
    String result);
void assert_streqn(int test_case, int called_at, String act, int exp_len, 
    String exp, int cmp_len);
void assert_newString(int test_case, int called_at, int len, char* buf);
int norm_split_test(int test_case, int called_at, char* str2split, char* delim, 
    char* exp_splits[], int num_splits);
int norm_substring_test(int test_case, int called_at, char* input, 
    int start, int ss_count, char** substrings);
    
/* test functions */
int test_newString() {
    int test_case = 0;
    
    char* buf;
    int len;

    for (int i = 0; i < 10; i++) {
        len = create_test_buf(-1, &buf);
        assert_newString(++test_case, __LINE__, len, buf);
        free(buf);
    }
    
    len = create_test_buf(STR_LEN_MAX * 2, &buf);
    assert_newString(++test_case, __LINE__, len, buf);
    free(buf);

    len = create_test_buf(STR_LEN_MAX / 2, &buf);
    assert_newString(++test_case, __LINE__, len, buf);
    free(buf);
    
    len = create_test_buf(STR_LEN_MAX, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX, buf);
    free(buf);
    
    len = create_test_buf(STR_LEN_MAX + 1, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX + 1, buf);
    free(buf);
   
    len = create_test_buf(STR_LEN_MAX - 1, &buf);
    assert_newString(++test_case, __LINE__, STR_LEN_MAX - 1, buf);
    free(buf);

    len = create_test_buf(1, &buf);
    assert_newString(++test_case, __LINE__, 1, buf);
    free(buf);
    
    assert_newString(++test_case, __LINE__, 0, "");

    String null_s = newString(NULL);
    
    assert_null(++test_case, __LINE__, null_s);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    return test_case;   
}

int test_deleteString() { 
    int test_case = 0;

    String s = create_test_str(-1);
    
    assert_notnull(++test_case, __LINE__, s);
    
    deleteString(&s);
    
    assert_null(++test_case, __LINE__, s);
    
    deleteString(&s);       /* no-op, will cause segmentation fault if not 
                             * implemented correctly 
                             */    
    ++test_case;            /* passed */
    
    deleteString(NULL);     /* no-op 
                             * will cause failure if not implemented correctly 
                             */
    ++test_case;            /* passed */
    
    return test_case;   
}

int test_char_at_norm() {
    int test_case = 0;
    
    char *buf;
    
    for (int i = 0; i < 10; i++) {
        ++test_case;
        int len = create_test_buf(-1, &buf);
        
        int len_s = len > STR_LEN_MAX ? STR_LEN_MAX : len;
        
        String s = newString(buf);
        
        assert(s);
        assert(len_s == s->length(s));
        
        for (int j = 0; j < len_s; j++)
            assert_eq(test_case, __LINE__, s->char_at(s, j), buf[j]);
        
        free(buf);
        deleteString(&s);
    }
    
    String s = newString("");
    
    assert(s);
    
    errno = 0;
    char c = s->char_at(s, 0);
    
    assert_eq(++test_case, __LINE__, c, 0);
    assert_eq(++test_case, __LINE__, errno, 0);
    
    deleteString(&s);
    
    return test_case;
}

int test_char_at_err() {
    int test_case = 0;
    
    String s = create_test_str(-1);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, -1), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, s->length(s)), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(s, s->length(s) + 1), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->char_at(NULL, 0), 0);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    
    deleteString(&s);
    
    return test_case;
}

int test_concat_norm() {
    int test_case = 0;
    
    String empty = newString("");
    assert(empty);
    assert(empty->length(empty) == 0);
    
   for (int i = 0; i < 20; i++) {
        String s1 = create_test_str(-1);
        String s2 = create_test_str(-1);

        String s3 = s1->concat(s1, s2);
        assert_concat_success(++test_case, __LINE__, s1, s2, s3);
        deleteString(&s3);
        
        s3 = s2->concat(s2, s1);
        assert_concat_success(++test_case, __LINE__, s2, s1, s3);
        deleteString(&s3);
        
        s3 = s1->concat(s1, s1);
        assert_concat_success(test_case, __LINE__, s1, s1, s3);
        deleteString(&s3);
        
        s3 = s1->concat(s1, empty);
        assert_concat_success(test_case, __LINE__, s1, empty, s3);
        deleteString(&s3);

        s3 = empty->concat(empty, s1);
        assert_concat_success(test_case, __LINE__, empty, s1, s3);
       
        deleteString(&s1);
        deleteString(&s2);
        deleteString(&s3);
    }

    String s1 = create_test_str(STR_LEN_MAX);
    String s2 = create_test_str(10);
    
    String s3 = s1->concat(s1, s2);
    assert_streqn(++test_case, __LINE__, s3, STR_LEN_MAX, s1, STR_LEN_MAX);
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s1),
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, s2, s3);
    
    deleteString(&s3);
    
    s3 = s2->concat(s2, s1);
    assert_streqn(++test_case, __LINE__, s3, STR_LEN_MAX, s2, 10);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s2), 
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    
    deleteString(&s3);       

    s3 = empty->concat(empty, s2);
    assert_streqn(++test_case, __LINE__, s3, 10, s2, 10);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s2), 
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s3);

    s3 = s2->concat(s2, empty);
    assert_streqn(++test_case, __LINE__, s3, 10, s2, 10);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s2), 
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);

    deleteString(&s2);
    deleteString(&s3);
    
    s2 = newString("");
    assert(s2);
    
    s3 = empty->concat(empty, s2);
    assert_streqn(++test_case, __LINE__, s3, 0, s2, 1);
    assert_streqn(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s2), 
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__,
        _test_string_val(empty), _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    deleteString(&s3);
    
    s3 = s2->concat(s2, empty);
    assert_streqn(++test_case, __LINE__, s3, 0, s2, 1);
    assert_streqn(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s2), 
        _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__,
        _test_string_val(empty), _test_string_val(s3));
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    deleteString(&s3);
    
    s3 = empty->concat(empty, empty);
    assert_streqn(++test_case, __LINE__, s3, 0, empty, 1);
    assert_notidentical(++test_case, __LINE__, empty, s3);
    assert_notidentical(++test_case, __LINE__,           
        _test_string_val(empty), _test_string_val(s3));
    
    deleteString(&s1);
    deleteString(&s2);
    deleteString(&s3);
    
    s1 = newString("a");
    s2 = newString("b");
    s3 = s1->concat(s1, s2);
    String s4 = newString("ab");
    assert_streqn(++test_case, __LINE__, s3, 2, s4, 2);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s3), 
        _test_string_val(s4));
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    assert_notidentical(++test_case, __LINE__, s3, s4);
    
    deleteString(&s3);
    deleteString(&s4);

    s3 = s2->concat(s2, s1);
    s4 = newString("ba");
    assert_streqn(++test_case, __LINE__, s3, 2, s4, 2);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s3), 
        _test_string_val(s4));
    assert_notidentical(++test_case, __LINE__, s1, s3);
    assert_notidentical(++test_case, __LINE__, s2, s3);
    assert_notidentical(++test_case, __LINE__, s3, s4);
        
    deleteString(&s1);
    deleteString(&s2);
    deleteString(&s3);       
    deleteString(&s4);
    deleteString(&empty);

    return test_case;
}

int test_concat_err() {
    int test_case = 0;
    String s = newString("test");
    assert_notnull(++test_case, __LINE__, s);
    assert_eq(++test_case, __LINE__, s->length(s), 4);
 
    errno = 0;
    String t = s->concat(s, NULL);    
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    t = s->concat(NULL, s);
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    t = s->concat(NULL, NULL);
    assert_null(++test_case, __LINE__, t);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    
    deleteString(&s);

    return test_case;
}

int test_equals() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);
        String t = newString(_test_string_val(s));
        
        assert(s);
        assert(t);
    
        assert_streqn(++test_case, __LINE__, s, s->length(s), t, t->length(t));
        
        assert_true(++test_case, __LINE__, s->equals(s, t));
        assert_true(++test_case, __LINE__, t->equals(t, s));
    
        assert_true(++test_case, __LINE__, s->equals(s, s));
        assert_true(++test_case, __LINE__, t->equals(t, t));
   
        deleteString(&s);
        deleteString(&t);
    }
    
    String s = newString("hello");
    String t = newString("goodbye");
    String u = newString("hellohello");
    
    assert(s);
    assert(t);
    assert(u);
    assert_false(++test_case, __LINE__, s->equals(s, t));
    assert_false(++test_case, __LINE__, t->equals(t, s));
    assert_false(++test_case, __LINE__, s->equals(s, u));
    assert_false(++test_case, __LINE__, s->equals(u, s));
    
    assert_false(++test_case, __LINE__, s->equals(s, NULL));
    assert_false(++test_case, __LINE__, s->equals(NULL, s));

    assert_false(++test_case, __LINE__, s->equals(NULL, NULL));
    
    
    deleteString(&s);
    deleteString(&t);
    deleteString(&u);
    
    s = newString("");
    t = newString("");
    
    assert(s);
    assert(t);
    assert_true(++test_case, __LINE__, s->equals(s, t));
    assert_true(++test_case, __LINE__, t->equals(t, s));
    
    deleteString(&s);
    deleteString(&t);

    return test_case;
}

int test_get_value_norm() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);
        
        char buf[s->length(s) + 1];
        
        char* val = s->get_value(s, buf);

        assert_notidentical(++test_case, __LINE__, val,
            _test_string_val(s));
        assert_notidentical(++test_case, __LINE__, buf,
            _test_string_val(s));
        assert_identical(++test_case, __LINE__, val, buf);
        assert_eq(++test_case, __LINE__, 
                strnlen(buf, s->length(s) + 1), s->length(s));

        assert_eq(++test_case, __LINE__, strncmp(val,
            _test_string_val(s), s->length(s)), 0);

        String t = newString(val);
                
        assert_true(++test_case, __LINE__, s->equals(s, t));
    
        deleteString(&s);
        deleteString(&t);
    }
    
    String s = newString("");
    assert(s);
    
    char buf[1];
    
    char *val = s->get_value(s, buf);
    
    assert_eq(++test_case, __LINE__, val[0], 0);
    
    deleteString(&s);
    
    char hello[] = "hello";
    
    s = newString(hello);
    assert(s);
    
    val = s->get_value(s, hello);
    
    assert_identical(++test_case, __LINE__, val, hello);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s),
        val);
    
    for (int i = 0; i < strlen(hello); i++)
        assert_eq(++test_case, __LINE__, val[i], hello[i]);
    
    deleteString(&s);

    char goodbye[] = "goodbye";
    
    s = newString(goodbye);
    assert(s);
    
    char gb_buf[strlen(goodbye) + 1];
    
    val = s->get_value(s, gb_buf);
    
    assert_identical(++test_case, __LINE__, val, gb_buf);
    assert_notidentical(++test_case, __LINE__, _test_string_val(s),
        val);
    assert_notidentical(++test_case, __LINE__, val, goodbye);
    
    for (int i = 0; i < strlen(goodbye); i++)
        assert_eq(++test_case, __LINE__, val[i], goodbye[i]);
    
    deleteString(&s);
    
    errno = 0; 
    val = NULL;
    char* ns = "new_string";    
    s = newString(ns);
    assert(s);
    
    val = s->get_value(s, NULL);
    
    assert_notnull(++test_case, __LINE__, val);
    
    assert_notidentical(++test_case, __LINE__, _test_string_val(s),
        val);
    assert_notidentical(++test_case, __LINE__, val, ns);
    assert_eq(++test_case, __LINE__, errno, 0);
    
    for (int i = 0; i < strlen(ns); i++)
        assert_eq(++test_case, __LINE__, val[i], ns[i]);
        
    deleteString(&s);
    free(val);

   return test_case;
}

int test_get_value_err()  {
    int test_case = 0;

    String s = create_test_str(-1);
    assert(s);
    
    char buf[s->length(s) + 1];
    
    errno = 0;
    assert_notnull(++test_case, __LINE__, s->get_value(s, buf));
    assert_eq(++test_case, __LINE__, errno, 0);

    errno = 0;
    assert_null(++test_case, __LINE__, s->get_value(NULL, buf));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    char* new_buf = s->get_value(s, NULL);
    assert_notnull(++test_case, __LINE__, new_buf);
    assert_eq(++test_case, __LINE__, errno, 0);
    free(new_buf);
    
    errno = 0;
    assert_null(++test_case, __LINE__, s->get_value(NULL, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    
    deleteString(&s);
    
    return test_case;
}

int test_index_of_norm() {
    int test_case = 0;
    
    String s = newString("hello");
    assert(s);
    
    assert_eq(++test_case, __LINE__, s->index_of(s, 'h', 0), 0);
    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'h', 1), -1);
    assert_eq(++test_case, __LINE__, errno, 0);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 0), 2);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 2), 2);
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 3), 3);
    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'l', 4), -1);
    assert_eq(++test_case, __LINE__, errno, 0);

    deleteString(&s);
    
    return test_case;
}

int test_index_of_err() {
    int test_case = 0;
    
    String s = create_test_str(-1);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(s, 'a', -1), -2);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, 
                s->index_of(s, 'a', s->length(s)), -2);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, 
                s->index_of(s, 'a', s->length(s) + 1), -2);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_eq(++test_case, __LINE__, s->index_of(NULL, '1', 0), -2);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    
    deleteString(&s);

    return test_case;
}

int test_length() {
    int test_case = 0;
    
    for (int i = 0; i < 10; i++) {
        char* buf;
        test_case++;
        
        int len = create_test_buf(-1, &buf);
        
        int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;
        
        String s = newString(buf);
        assert(s);
                
        assert_eq(test_case, __LINE__, s->length(s), exp_len);
        
        free(buf);
        
        deleteString(&s);
    }
        
    String s = newString(" ");
    assert(s);
    
    assert_eq(++test_case, __LINE__, s->length(s), 1);
    
    errno = 0;
    assert_eq(++test_case, __LINE__, s->length(NULL), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    errno = 0;
    
    deleteString(&s);
    
    s = newString("");
    assert(s);
    assert_eq(++test_case, __LINE__, s->length(s), 0);
    deleteString(&s);
    
    return test_case;
}

int test_split_norm()  {
    int test_case = 0;
    
    char* delim = ":,+";
    char* str2split = "noseparator";
    char* exp_splits1[] = {"noseparator"};
    int num_splits = 1;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits1, num_splits);
    
    str2split = "three:parts:str";
    char* exp_splits2[] = {"three", "parts", "str"};
    num_splits = 3;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits2, num_splits);
    
    str2split = ":";
    char* exp_splits3[] = {"", ""};
    num_splits = 2;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits3, num_splits);

    str2split = "::";
    char* exp_splits4[] = {"", "", ""};
    num_splits = 3;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits4, num_splits);

    str2split = "+++";
    char* exp_splits5[] = {"", "", "", ""};
    num_splits = 4;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits5, num_splits);

    str2split = ",,,,";
    char* exp_splits6[] = {"", "", "", "", ""};
    num_splits = 5;
    test_case = norm_split_test(test_case, __LINE__, str2split, delim,
        exp_splits6, num_splits);

    str2split = "";
    char* exp_splits7[] = {""};
    num_splits = 1;
    test_case = norm_split_test(test_case, __LINE__, str2split , delim,
        exp_splits7, num_splits);
        
    str2split = "a,b:c+d";
    char* exp_splits8[] = {"a", "b", "c", "d"};
    num_splits = 4;
    test_case = norm_split_test(test_case, __LINE__, str2split , delim,
        exp_splits8, num_splits);
        
    str2split = ":a,b:c+d+";
    char* exp_splits9[] = {"", "a", "b", "c", "d", ""};
    num_splits = 6;
    test_case = norm_split_test(test_case, __LINE__, str2split , delim,
        exp_splits9, num_splits);
        
    str2split = "abc";
    char* exp_splits10[] = {"abc"};
    num_splits = 1;
    test_case = norm_split_test(test_case, __LINE__, str2split , "",
        exp_splits10, num_splits);

    return test_case;
}

int test_split_err()  {
    int test_case = 0;
    
    String s = newString("s");
    assert(s);
    String d = newString("d");
    assert(d);
    
    String* splits = s->split(s, NULL);
    assert_null(++test_case, __LINE__, splits);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    splits = s->split(NULL, d);
    assert_null(++test_case, __LINE__, splits);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    splits = s->split(NULL, NULL);
    assert_null(++test_case, __LINE__, splits);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    deleteString(&s);
    assert(!s);
    deleteString(&d);
    assert(!d);
    
    return test_case;
}

int test_substring_norm()  {
    int test_case = 0;
    
    char* empty_subs[] = {""};
    test_case = norm_substring_test(test_case, __LINE__, "", 0, 0, empty_subs);
    
    char* input = "input";
    
    char* substring0[] = {"", "i", "in", "inp", "inpu", input};
    test_case = norm_substring_test(test_case, __LINE__, input, 0,
        strlen(input) + 1, substring0);
    
    char* substring1[] = {"", "n", "np", "npu", "nput"};
    test_case = norm_substring_test(test_case, __LINE__, input, 1,
        strlen(input), substring1);

    char* substring2[] = {"", "p", "pu", "put"};
    test_case = norm_substring_test(test_case, __LINE__, input, 2,
        strlen(input) - 1, substring2);

    char* substring3[] = {"", "u", "ut"};
    test_case = norm_substring_test(test_case, __LINE__, input, 3,
        strlen(input) - 2, substring3); 

    char* substring4[] = {"", "t"};
    test_case = norm_substring_test(test_case, __LINE__, input, 4,
        strlen(input) - 3, substring4);
    
    char* substring5[] = {""};
    test_case = norm_substring_test(test_case, __LINE__, input, 5,
        strlen(input) - 4, substring5);

    return test_case;
}

int test_substring_err()  {
    int test_case = 0;

    String s = newString("");
    assert(s);
    String ss = s->substring(s, 0, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, -1, 0);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    ss = s->substring(s, -1, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    deleteString(&s);
    assert(!s);

    s = newString("hello");
    assert(s);
    int slen = s->length(s);
    
    errno = 0;
    ss = s->substring(NULL, 0, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, -1, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, -1, 0);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    ss = s->substring(s, slen, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, slen + 1, 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, 0, -1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, 0, slen + 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, 1, slen);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    ss = s->substring(s, 2, slen - 1);
    assert_null(++test_case, __LINE__, ss);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    deleteString(&s);
    assert(!s);

    return test_case;
}

void assert_concat_success(int test_case, int called_at, String lhs, String rhs, 
    String result) {
    assert_notnull_ca(test_case, __LINE__, called_at, result);
    assert_notidentical_ca(test_case, __LINE__, called_at, lhs, result);
    assert_notidentical_ca(test_case, __LINE__, called_at, rhs, result);
    
    int exp_len = lhs->length(lhs) + rhs->length(rhs);
    
    exp_len = exp_len > STR_LEN_MAX ? STR_LEN_MAX : exp_len;
    
    assert_eq_ca(test_case, __LINE__, called_at, 
        result->length(result), exp_len);

    char buf[STR_LEN_MAX + STR_LEN_MAX + 1];
    
    strncpy(buf, _test_string_val(lhs), lhs->length(lhs));
    
    buf[lhs->length(lhs)] = '\0';
    
    strncat(buf, _test_string_val(rhs), rhs->length(rhs));
    
    buf[result->length(result)] = '\0';
    
    assert_eq_ca(test_case, __LINE__, called_at,
        strncmp(buf, _test_string_val(result), STR_LEN_MAX), 0);
}

void assert_streqn(int test_case, int called_at, String act, int exp_len, 
    String exp, int cmp_len) {
    assert_notnull_ca(test_case, __LINE__, called_at, act);
    assert_eq_ca(test_case, __LINE__, called_at, act->length(act), exp_len);
    assert_eq_ca(test_case, __LINE__, called_at, strncmp(_test_string_val(act),
        _test_string_val(exp), cmp_len), 0);
}

void assert_newString(int test_case, int called_at, int len, char* buf) {
    int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;
                
    String s = newString(buf);
    
    assert_notnull_ca(test_case, __LINE__, called_at, s);
    assert_notnull_ca(test_case, __LINE__, called_at, _test_string_val(s));
    assert_eq_ca(test_case, __LINE__, called_at, s->length(s), exp_len);
    assert_eq_ca(test_case, __LINE__, called_at, 
        strlen(_test_string_val(s)), exp_len);
    assert_eq_ca(test_case, __LINE__, called_at, 
        strncmp(buf, _test_string_val(s), exp_len),
                0);
    assert_notidentical_ca(test_case, __LINE__, called_at, buf,
        _test_string_val(s));
    
    assert_notnull_ca(test_case, __LINE__, called_at, s->char_at);
    assert_notnull_ca(test_case, __LINE__, called_at, s->concat);
    assert_notnull_ca(test_case, __LINE__, called_at, s->equals);
    assert_notnull_ca(test_case, __LINE__, called_at, s->get_value);
    assert_notnull_ca(test_case, __LINE__, called_at, s->index_of);
    assert_notnull_ca(test_case, __LINE__, called_at, s->length);   
    assert_notnull_ca(test_case, __LINE__, called_at, s->split);   
    assert_notnull_ca(test_case, __LINE__, called_at, s->substring);   
        
    deleteString(&s);
}

int norm_split_test(int test_case, int called_at, char* str2split, char* delim, 
    char* exp_splits[], int num_splits) {
    errno = 0;
    String s2s = newString(str2split);
    String ds = newString(delim);
    assert(s2s);
    assert(ds);
    
    String* splits = s2s->split(s2s, ds);
    
    assert_notnull_ca(++test_case, __LINE__, called_at, splits);
    
    const char* s2sval = _test_string_val(s2s);
    
    for (int i = 0; i < num_splits; i++) {
        String exp_s = newString(exp_splits[i]);
        assert(exp_s);
        assert_notnull_ca(++test_case, __LINE__, called_at, splits[i]);
        assert_streqn(++test_case, called_at, splits[i], 
            splits[i]->length(splits[i]), exp_s, exp_s->length(exp_s));
        assert_notidentical_ca(++test_case, __LINE__, called_at, 
            splits[i], exp_s);
        
        const char* split = _test_string_val(splits[i]);
        
        assert_true(++test_case, __LINE__, s2sval != split);
        
        deleteString(&exp_s);
        assert(!exp_s);
    }

    assert_null_ca(++test_case, __LINE__, called_at, splits[num_splits]);
        
    assert_eq_ca(++test_case, __LINE__, called_at, 
        strncmp(s2sval, str2split, strlen(str2split)), 0);
    assert_eq_ca(++test_case, __LINE__, called_at, 
        strncmp(_test_string_val(ds), delim, strlen(delim)), 0);
    
    for (int i = 0; i < num_splits; i++) {
        deleteString(&splits[i]);
        assert(!splits[i]);
    }
    
    free(splits);
    
    deleteString(&s2s);
    deleteString(&ds);
    
    assert_eq_ca(++test_case, __LINE__, called_at, errno, 0);
    
    return test_case;
}

int norm_substring_test(int test_case, int called_at, char* input, 
    int start, int ss_count, char** substrings) {
    String ins = newString(input);
    assert(ins);
    
    for (int len = 0; len < ss_count; len++) {
        errno = 0;
        String ss = ins->substring(ins, start, len);
        assert_notnull_ca(++test_case, __LINE__, called_at, ss);
        int sslen = ss->length(ss);
    
        assert_eq_ca(++test_case, __LINE__, called_at, sslen, len);
    
        assert_eq_ca(++test_case, __LINE__, called_at, 
            strncmp(_test_string_val(ss), substrings[len], len), 0);
            
        assert_true(++test_case, __LINE__, ss != ins);
        
        deleteString(&ss);
        assert(!ss);
    
        assert_eq_ca(++test_case, __LINE__, called_at, errno, 0);
    }
    
    assert_eq_ca(++test_case, __LINE__, called_at, 
        strncmp(_test_string_val(ins), input, strlen(input)), 0);

    deleteString(&ins);
    assert(!ins);

    return test_case;
}




