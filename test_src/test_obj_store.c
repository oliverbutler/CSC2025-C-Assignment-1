#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "test_lib.h"
#include "strtest_lib.h"
#include "../string_o.h"
#include "../integer.h"
#include "../obj_store.h"

static char* OFILE_FMT = "./%s/%s/%#zx.txt";
static char* OSTORE_DIR = "ostore";

#define NR_TESTS 3

/* test functions */
int test_enable_is_on();
int test_store_unlink_norm();
int test_store_unlink_err();

struct test_defn test_schedule[NR_TESTS] = {
    /* test 0 */
    { "test_ostore_enable", test_enable_is_on, 3, 0 },
    /* test 1 */
    { "test_ostore_unlink_norm", test_store_unlink_norm, 169, 0 },
    /* test 2 */
    { "test_ostore_unlink_err", test_store_unlink_err, 6, 0 }
};

/* helper functions */
int assert_written(int test_case, int called_at, const char* type, 
    uintptr_t oid, char* data);
int assert_unlinked(int test_case, int called_at, char* ofile);
object_rep* _new_dummy_obj_rep(void* obj);

int main(int argc, char** argv) {
    run_tests(argc, argv, NR_TESTS, test_schedule, true);
    
    return 0;
}

int test_enable_is_on() {
    int test_case = 0;
    
    if (enable_ostore()) {
        assert_true(++test_case, __LINE__, ostore_is_on());
        struct stat sbuf;

        int r = stat(OSTORE_DIR, &sbuf);
        assert_eq(++test_case, __LINE__, r, 0);
        assert_true(++test_case, __LINE__, S_ISDIR(sbuf.st_mode));
    } else {
        perror("Could not create ostore");
        exit(EXIT_FAILURE);
    }

    return test_case;
}

int test_store_unlink_norm() {
    int test_case = 0;
    char *ofile = NULL;
    
    errno = 0;
    if (!ostore_is_on() && !enable_ostore()) {
        perror("Could not enable ostore");
        exit(EXIT_FAILURE);
    }        

    object_rep* obj_rep = _new_dummy_obj_rep(&test_case);
    bool stored = store_obj(obj_rep);
    assert_true(++test_case, __LINE__, stored);
    test_case = assert_written(test_case, __LINE__, obj_rep->type, 
        obj_rep->id, "test");

    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, obj_rep->type, obj_rep->id);
    unlink_obj(obj_rep);
    test_case = assert_unlinked(test_case, __LINE__, ofile);
    free((void*) obj_rep->valstr);
    free(obj_rep);
    
    char* str_rep;
    
    for (int i = 0; i < 10; i++) {
        String s = create_test_str(-1);
        assert(s);
    
        char sbuf[s->length(s) + 1];
    
        (void) asprintf(&str_rep, "%d:%s\n", s->length(s), 
                s->get_value(s, sbuf)); 
    
        assert(str_rep);
    
        test_case = assert_written(test_case, __LINE__, "str", (uintptr_t) s,
            str_rep);
        
        (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, "str", s);
    
        free(str_rep);
        deleteString(&s);
        
        test_case = assert_unlinked(test_case, __LINE__, ofile);
    }
    
    for (int i = 0; i < 10; i++) {
        Integer oi = newInteger(rand());
        assert(oi);
        
        (void) asprintf(&str_rep, "%d\n", oi->get_value(oi));
    
        assert(str_rep);
    
        test_case = assert_written(test_case, __LINE__, "int", (uintptr_t) oi,        
                str_rep);
        
        (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, "int", oi);
        
        free(str_rep);
        deleteInteger(&oi);
        
        test_case = assert_unlinked(test_case, __LINE__, ofile);
    }
    
    return test_case;
}

int test_store_unlink_err() {
    int test_case = 0;

    errno = 0;
    if (!ostore_is_on() && !enable_ostore()) {
        perror("Could not enable ostore");
        exit(EXIT_FAILURE);
    }        

    errno = 0;
    assert_false(++test_case, __LINE__, store_obj(NULL));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    object_rep* obj_rep = _new_dummy_obj_rep(&test_case);
    assert(obj_rep);
    free((void*) obj_rep->valstr);
    obj_rep->valstr = NULL;
    errno = 0;
    assert_false(++test_case, __LINE__, store_obj(obj_rep));
    assert_eq(++test_case, __LINE__, errno, EINVAL);
    
    char *ofile = NULL;
    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, obj_rep->type, obj_rep->id);
    test_case = assert_unlinked(test_case, __LINE__, ofile);
    
    errno = 0;
       
    unlink_obj(NULL);   /* no-op, fails if not implemented correctly */
                             
    errno = 0;
    obj_rep->type = NULL;
    unlink_obj(obj_rep);   /* no-op, fails if not implemented correctly */

    free(obj_rep);
     
    return test_case;
}

/* helper functions */
int assert_written(int test_case, int called_at, const char* type, 
    uintptr_t oid, char* data) {
    char* ofile = NULL;
    size_t data_len = strlen(data);
    struct stat sbuf;
    
    (void) asprintf(&ofile, OFILE_FMT, OSTORE_DIR, type, oid);
    
    assert_notnull_ca(++test_case, __LINE__, called_at, ofile);
    
    int r = stat(ofile, &sbuf);
    
    assert_eq_ca(++test_case, __LINE__, called_at, r, 0);
    assert_true_ca(++test_case, __LINE__, called_at, S_ISREG(sbuf.st_mode));
    
    int fd = open(ofile, O_RDONLY, 0644);

    free(ofile);
    
    assert_true_ca(++test_case, __LINE__, called_at, fd > 0);
    
    char rbuf[data_len + 1];
    
    ssize_t bytes_read = read(fd, rbuf, data_len);

    close(fd);

    assert_eq_ca(++test_case, __LINE__, called_at, bytes_read, data_len);
    
    rbuf[bytes_read] = '\0';
    
    assert_eq_ca(++test_case, __LINE__, called_at, 
        strncmp(rbuf, data, data_len), 0);
            
    return test_case;
}

int assert_unlinked(int test_case, int called_at, char* ofile) {
    struct stat sbuf;
    
    errno = 0;
    int r = stat(ofile, &sbuf);

    assert_eq_ca(++test_case, __LINE__, called_at, r, -1);
    assert_eq_ca(++test_case, __LINE__, called_at, errno, ENOENT);
    
    free(ofile);
    
    errno = 0;
    
    return test_case;
}

/* create dummy string rep for tests */
char* TYPE_STR = "obj";

object_rep* _new_dummy_obj_rep(void* obj) {
    assert(obj);
    object_rep* obj_rep = (object_rep*) malloc(sizeof(object_rep));
    
    assert(obj_rep);
    
    obj_rep->type = TYPE_STR;
    obj_rep->id = (uintptr_t) obj;
    
    char* valstr = NULL;
    
    (void) asprintf(&valstr, "%s",  "test");
    
    assert(valstr);
        
    obj_rep->valstr = valstr;

    return obj_rep;
}





