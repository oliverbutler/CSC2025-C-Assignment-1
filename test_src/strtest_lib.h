#ifndef _STRTEST_LIB_H
#define _STRTEST_LIB_H
#include "../string_o.h"

/* declaration of utility functions for string (and obj_store) tests */
      
int create_test_buf(int len, char** out_buf);

String create_test_str(int len);

#endif