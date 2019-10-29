#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "strtest_lib.h"
#include "test_lib.h"

/* implementation of utility functions for string (and obj_store) tests */

#define ALPHA_LEN 26
static char* alphabet = "abcdefghijklmnopqrstuvwxyz";

int create_test_buf(int len, char** out_buf) {
    if (len == -1) {
        int r = rand();
        len = r % 2 ? r % (STR_LEN_MAX * 2) : r % (STR_LEN_MAX / 2);
    } else if (len < 1) {
        len = STR_LEN_MAX;
    }
    
    char* buf = (char *) calloc(len + 1, 1);
    
    for (int i = 0; i < len; i++)
        buf[i] = alphabet[rand() % ALPHA_LEN];
    
    buf[len] = '\0';
    
    *out_buf = buf;
    
    return len;
}

String create_test_str(int len) {
    char* buf;
    
    len = create_test_buf(len, &buf);
    
    String s = newString(buf);
    
    int exp_len = len > STR_LEN_MAX ? STR_LEN_MAX : len;
    
    assert(s);
    assert(exp_len == s->length(s));
    assert(buf != _test_string_val(s));
    assert(strncmp(buf, _test_string_val(s), exp_len) == 0);
    
    free(buf);
        
    return s;
}

