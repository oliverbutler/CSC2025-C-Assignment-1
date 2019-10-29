#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include "../integer.h"

#include <assert.h>

/* 
 * Program to demonstrate example usage of Integer (as defined in integer.h).
 */

int main(void) {
    errno = 0;

    Integer i = newInteger(1);  /* strictly, should check this succeeds */
    Integer j = newInteger(2);  /* strictly, should check this succeeds */

    Integer k = i->add(i, j);   /* should succeed */
   
    if (k) {
        printf("result of k = i->add(i, j): %d + %d = %d\n",
            i->get_value(i), j->get_value(j), k->get_value(k));
    } else {
         perror("i + j failed.");
         exit(EXIT_FAILURE);
    }
    
    Integer imax = newInteger(INT_MAX);  
                                /* strictly, should check this succeeds */    
    
    printf("Value of im is: %d\n", imax->get_value(imax));
    
    Integer l = i->add(i, imax);    /* should cause error */
    
    if (l) {
        printf("result of m = i->add(i, im): %d + %d = %d\n", 
                i->get_value(i), imax->get_value(imax), l->get_value(l));
    } else {
        perror("i + imax failed.");
    }    
    
    printf("values of of i, j, k, imax and l pointers before deleting:\n");
    printf("pointer i:    %#zx\n", (size_t) i);
    printf("pointer j:    %#zx\n", (size_t) j);
    printf("pointer k:    %#zx\n", (size_t) k);
    printf("pointer imax: %#zx\n", (size_t) imax);
    printf("pointer l:    %#zx\n", (size_t) l);

    deleteInteger(&i);
    deleteInteger(&j);
    deleteInteger(&k);
    deleteInteger(&imax);
    deleteInteger(&l);

    printf("values of i, j, k, imax and l pointers after deleting:\n");
    printf("pointer i:    %#zx\n", (size_t) i);
    printf("pointer j:    %#zx\n", (size_t) j);
    printf("pointer k:    %#zx\n", (size_t) k);
    printf("pointer imax: %#zx\n", (size_t) imax);
    printf("pointer l:    %#zx\n", (size_t) l);   

    return 0;
} 