#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "obj_map.h"

#include <stdio.h>

/* implementation of object map. See obj_map.h for the specification of an 
 * object map and its functions.  
 * Do NOT change anything in this file.
 */

/* definition of a key/value node/entry in a map */
typedef struct omap_node {
    void* key;
    void* val;
    struct omap_node* next;
} omap_node;

/* definition of a map */
struct omap {
    int nbuckets;
    int nentries;
    omap_node** buckets;
};

/* hash code to determine bucket corresponding to given key */
int hash_code(omap* m, void* key) {
    return (int) ((uintptr_t) key %  m->nbuckets);
}

/* see obj_map.h */
omap* create_map() {
    return create_map_wbuckets(OMAP_DEFAULT_NBUCKETS);
}

/* see obj_map.h */
omap* create_map_wbuckets(int nbuckets) {
    if (nbuckets < 1) {
        errno = EINVAL;
        return NULL;
    }
    
    omap* m = (omap*) malloc(sizeof(omap));
    
    if (m) {
        m->nbuckets = nbuckets;
        m->nentries = 0;
        m->buckets = (omap_node**) calloc(nbuckets, sizeof(omap_node*));
        
        if (!m->buckets) {
            free(m);
            m = NULL;
        }
    }

    return m;
}
 
/* see obj_map.h */
void delete_map(omap** m) {
    if (m && *m) {
        for (int i = 0; i < (*m)->nbuckets; i++) {
            omap_node* n = (*m)->buckets[i];
        
            while (n) {
                omap_node* p = n;
                n = n->next;
                free(p);
            }
        
        }
    
        free((*m)->buckets);
        free(*m);
        *m = NULL;
    }
}

/* see obj_map.h */
void* delete_mentry(omap* m, void* key) {
    if (!m || !key) {
        errno = EINVAL;
        return NULL;
    }
    
    int hc = hash_code(m, key);
    
    omap_node* n = m->buckets[hc];
    omap_node* p = NULL;
    void* val = NULL;

    while (n && n->key != key) {
        p = n;
        n = n->next;
    }
    
    if (n) {
        val = n->val;
        if (p) {
            p->next = n->next;
        } else {
            m->buckets[hc] = n->next;
        }
        
        m->nentries--;
        
        free(n);
    } else {
        errno = EINVAL;
    }
    
    return val;
}

/* see obj_map.h */
void* get_mentry(omap* m, void* key) {
    if (!m || !key) {
        errno = EINVAL;
        return NULL;
    }
    
    int hc = hash_code(m, key);
    
    omap_node* n = m->buckets[hc];
    
    while (n && n->key != key)
        n = n->next;
    
    if (!n) {
        errno = EINVAL;
        return NULL;
    }
    
    return n->val;
}

/* see obj_map.h */
int get_numbuckets(omap* m) {
    if (!m) {
        errno = EINVAL;
        return -1;
    }
    
    return m->nbuckets;
}

/* see obj_map.h */
int get_numentries(omap* m) {
    if (!m) {
        errno = EINVAL;
        return -1;
    }
    
    return m->nentries;
}

/* see obj_map.h */
bool set_mentry(omap* m, void* key, void* val) {
    if (!m || !key || !val) {
        errno = EINVAL;
        return false;
    }
    
    int hc = hash_code(m, key);
    
    omap_node* n = m->buckets[hc];
    
    while (n) {
        if (n->key == key) {
            n->val = val;
            return true;
        }
        n = n->next;
    }
    
    n = (omap_node*) malloc(sizeof(omap_node));
    
    if (n) {
        n->key = key;
        n->val = val;
        n->next = m->buckets[hc];
    
        m->buckets[hc] = n;
        
        m->nentries++;
        
        return true;
    }
    
    return false;
}


