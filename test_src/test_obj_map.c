#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "test_lib.h"
#include "strtest_lib.h"
#include "../obj_map.h"

#define NR_TESTS 7

/* test functions */
int test_create_map();
int test_delete_map();
int test_delete_mentry();
int test_get_mentry();
int test_get_numbuckets();
int test_get_numentries();
int test_set_mentry();

struct test_defn test_schedule[NR_TESTS] = {
    /* test 0 */
    { "test_create_map", test_create_map, 12, 0 },
    /* test 1 */
    { "test_delete_map", test_delete_map, 2, 0 },
    /* test 2 */
    { "test_delete_mentry", test_delete_mentry, 24, 0 },
    /* test 3 */
    { "test_get_mentry", test_get_mentry, 26, 0 },
    /* test 4 */
    { "test_get_numbuckets", test_get_numbuckets, 6, 0 },
    /* test 5 */
    { "test_get_numentries", test_get_numentries, 53, 0 },
    /* test 6 */
    { "test_set_mentry", test_set_mentry, 33, 0 },

};

/* helper functions */

int main(int argc, char** argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, true);
    
    return 0;
}

int test_create_map() {
    int test_case = 0;
    errno = 0;
    omap* m = create_map();

    assert_notnull(++test_case, __LINE__, m);
    assert_eq(++test_case, __LINE__, get_numbuckets(m), OMAP_DEFAULT_NBUCKETS);
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);
    assert_eq(++test_case, __LINE__, errno, 0);

    delete_map(&m);
    
    assert_null(++test_case, __LINE__, m);
    
    errno = 0;
    
    m = create_map_wbuckets(50);

    assert_notnull(++test_case, __LINE__, m);
    assert_eq(++test_case, __LINE__, get_numbuckets(m), 50);
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);
    assert_eq(++test_case, __LINE__, errno, 0);

    delete_map(&m);
    
    assert_null(++test_case, __LINE__, m);

    errno = 0;
    m = create_map_wbuckets(0);
    
    assert_null(++test_case, __LINE__, m);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    return test_case;
}

int test_delete_map() {
    int test_case = 0;
    omap* m = create_map();
    assert(m);    
    delete_map(&m);
    assert_null(++test_case, __LINE__, m);
    
    m = create_map_wbuckets(500);
    assert(m);    
    delete_map(&m);
    assert_null(++test_case, __LINE__, m);
    
    delete_map(NULL);
    
    return test_case;
}

int test_delete_mentry() {
    int test_case = 0;
    errno = 0;
    omap* m = create_map();

    assert(m);
    
    char* key[4] = {"k1", "k2", "k3", "k4"};
    int val[4] = {1, 2, 3, 4};
    
    for (int i = 0; i < 4; i++) {
        assert(set_mentry(m, key[i], &val[i]));
        int* mval = (int*) get_mentry(m, key[i]);
        assert(*mval = val[i]);
    }

    assert(get_numentries(m) == 4);
    
    for (int i = 0; i < 4; i++) {
        int* mval = (int*) delete_mentry(m, key[i]);
        assert_eq(++test_case, __LINE__, *mval, val[i]);
        assert_eq(++test_case, __LINE__, get_numentries(m), 3 - i);
    }
    
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);
    assert_eq(++test_case, __LINE__, errno, 0);
    
    for (int i = 0; i < 4; i++) {
        errno = 0;
        int* mval = (int*) get_mentry(m, key[i]);
        assert_null(++test_case, __LINE__, mval);
        assert_eq(++test_case, __LINE__, errno, EINVAL);
    }
    
    errno = 0;
    
    assert_null(++test_case, __LINE__, delete_mentry(m, key[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL);    
    
    assert_null(++test_case, __LINE__, delete_mentry(m, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);    

    delete_map(&m);
    assert(!m);
    
    errno = 0;
    assert_null(++test_case, __LINE__, delete_mentry(NULL, key[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL);    
        
    return test_case;
}

// in get_mentry test for get with unknown key and NULL key
int test_get_mentry() {
    int test_case = 0;
    errno = 0;
    omap* m = create_map();

    assert(m);
    assert(get_numbuckets(m) == OMAP_DEFAULT_NBUCKETS);
    assert(get_numentries(m) == 0);
    
    char* key[4] = {"k1", "k2", "k3", "k4"};
    int val[4] = {1, 2, 3, 4};
    
    errno = 0;
    assert_null(++test_case, __LINE__, get_mentry(m, key[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL); 
    
    errno = 0;
    for (int i = 0; i < 4; i++) {
        assert(set_mentry(m, key[i], &val[i]));
        assert(get_numentries(m) == i+1);
     }
            
    for (int i = 0; i < 4; i++) {
        int* mval = (int*) get_mentry(m, key[i]);
        assert_eq(++test_case, __LINE__, *mval, val[i]);
        assert_eq(++test_case, __LINE__, errno, 0);
    }

    delete_map(&m);
    
    assert(!m);

    errno = 0;
    
    m = create_map_wbuckets(101);
    assert(m);
    assert(get_numbuckets(m) == 101);
    assert(get_numentries(m) == 0);
    
    char* key2[4] = {"k5", "k6", "k7", "k8"};
    int val2[4] = {5, 6, 7, 8};
    
    errno = 0;
    assert_null(++test_case, __LINE__, get_mentry(m, key[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL); 
    
    for (int i = 0; i < 4; i++) {
        assert(set_mentry(m, key2[i], &val2[i]));
        assert(get_numentries(m) == i+1);
    }

    errno = 0;
    for (int i = 0; i < 4; i++) {
        int* mval = (int*) get_mentry(m, key2[i]);
        assert_eq(++test_case, __LINE__, *mval, val2[i]);
        assert_eq(++test_case, __LINE__, errno, 0);
    }

    assert_null(++test_case, __LINE__, get_mentry(m, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    delete_map(&m);
    
    assert(!m);
    
    errno = 0;
    assert_null(++test_case, __LINE__, get_mentry(m, key2[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    assert_null(++test_case, __LINE__, get_mentry(m, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    return test_case;
}

int test_get_numbuckets() {
    int test_case = 0;
    errno = 0;

    omap* m = create_map();

    assert(m);
    assert_eq(++test_case, __LINE__, get_numbuckets(m), OMAP_DEFAULT_NBUCKETS);
    assert_eq(++test_case, __LINE__, errno, 0);

    delete_map(&m);
    
    assert(!m);

    m = create_map_wbuckets(20);

    assert(m);
    assert_eq(++test_case, __LINE__, get_numbuckets(m), 20);
    assert_eq(++test_case, __LINE__, errno, 0);

    delete_map(&m);
    
    assert(!m);
    
    errno = 0;
    
    assert_eq(++test_case, __LINE__, get_numbuckets(m), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    return test_case;
}

int test_get_numentries() {
    int test_case = 0;
    errno = 0;

    omap* m = create_map();

    assert(m);
    
    assert(get_numbuckets(m) == OMAP_DEFAULT_NBUCKETS);
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);
    
    char* key[4] = {"k1", "k2", "k3", "k4"};
    int val[4] = {1, 2, 3, 4};
    
    for (int i = 0; i < 4; i++) {
        assert(set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), i+1);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    for (int i = 0; i < 4; i++) {
        val[i]++;
        assert(set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), 4);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    for (int i = 0; i < 4; i++) {
        assert(delete_mentry(m, key[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), 3 - i);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);        
        
    delete_map(&m);
    
    assert(!m);
    
    m = create_map_wbuckets(20);

    for (int i = 0; i < 4; i++) {
        assert(set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), i+1);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    for (int i = 0; i < 4; i++) {
        val[i]++;
        assert(set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), 4);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    for (int i = 0; i < 4; i++) {
        assert(delete_mentry(m, key[i]));
        assert_eq(++test_case, __LINE__,  get_numentries(m), 3 - i);
        assert_eq(++test_case, __LINE__, errno, 0);
    }
    
    assert_eq(++test_case, __LINE__, get_numentries(m), 0);        
        
    delete_map(&m);
    
    assert(!m);
    
    errno = 0;
    
    assert_eq(++test_case, __LINE__, get_numentries(m), -1);
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    return test_case;
}

int test_set_mentry() {
    int test_case = 0;
    errno = 0;
    omap* m = create_map();

    assert(m);
    
    char* key[4] = {"k1", "k2", "k3", "k4"};
    int val[4] = {1, 2, 3, 4};
    
    for (int i = 0; i < 4; i++) {
        assert_true(++test_case, __LINE__, set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__, get_numentries(m), i+1);
        int* mval = (int*) get_mentry(m, key[i]);
        assert_eq(++test_case, __LINE__, *mval, val[i]);
    }
            
    for (int i = 0; i < 4; i++) {
        val[i]++;
        assert_true(++test_case, __LINE__, set_mentry(m, key[i], &val[i]));
        assert_eq(++test_case, __LINE__, get_numentries(m), 4);
    }

    for (int i = 0; i < 4; i++) {
        int* mval = get_mentry(m, key[i]);
        assert_eq(++test_case, __LINE__, *mval, val[i]);
    }
    
    assert_eq(++test_case, __LINE__, errno, 0);
    
    assert_false(++test_case, __LINE__, set_mentry(m, NULL, &val[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    errno = 0;
    assert_false(++test_case, __LINE__, set_mentry(m, key[0], NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    errno = 0;
    assert_false(++test_case, __LINE__, set_mentry(m, NULL, NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    delete_map(&m);
    
    assert(!m);

    errno = 0;
    
    assert_false(++test_case, __LINE__, set_mentry(m, key[0], &val[0]));
    assert_eq(++test_case, __LINE__, errno, EINVAL);

    return test_case;
}

