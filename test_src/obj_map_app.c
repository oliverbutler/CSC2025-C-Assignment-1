#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../obj_map.h"

/* 
 * Program to demonstrate example usage of object map (as defined in obj_map.h).
 */

int main(void) {
    omap* m = create_map(49);
    int key[10];

    for (int i = 0; i < 10; i++) {
        int* val = (int*) malloc(sizeof(int));
        *val = i;
        
        set_mentry(m, &key[i], val);
    }
        
    for (int i = 0; i < 10; i++)
        printf("Entry in map - key: %#zx, val: %d\n", (uintptr_t) &key[i],
                *(int*)get_mentry(m, &key[i]));
    

    for (int i = 0; i < 10; i++)
        printf("Entry deleted from map - key: %#zx, val: %d\n", 
                (uintptr_t) &key[i], *(int*)delete_mentry(m, &key[i]));
        
    for (int i = 0; i < 10; i++) {
        void* x =  get_mentry(m, &key[i]);
        if (x)
            printf("Error, entry for %#zx should be null\n", 
                (uintptr_t) &key[i]);
        else 
            printf("Entry for %#zx not in map, now null\n", 
                (uintptr_t) &key[i]);
    }
    
    delete_map(&m);
    return 0;
    
}