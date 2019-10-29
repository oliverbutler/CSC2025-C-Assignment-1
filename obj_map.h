#ifndef _OBJ_MAP_H
#define _OBJ_MAP_H
#include <stdbool.h>

/* specification of object map. Do NOT change anything in this file */

/* 
 * Default number of buckets in an object map. The number of buckets will
 * impact the length of hash map value lists. For example, if there is 
 * a single bucket then all objects will be stored in one linked list. 
 * The more buckets there are the more likely that objects will be stored in 
 * short lists, reducing the time taken to insert and retrieve objects. 
 * However, more buckets also means more memory is used for the hash map (and
 * the more likely it is that there is wasted space with slots for empty 
 * buckets).
 */
#define OMAP_DEFAULT_NBUCKETS 127

/* 
 * Declaration of the omap type for storage of a key/value mapping of objects.
 * Both keys and values are generic void* types.
 */
typedef struct omap omap;

/*
 * Function:
 * create_map()
 * 
 * Description:
 * Creates an object hash map with the default number of buckets
 * (OMAP_DEFAULT_NBUCKETS).
 *
 * Usage: 
 *      omap* map = create_map();   // creates map with OMAP_DEFAULT_NBUCKETS 
 *      ...                         // buckets
 *      ...
 *      delete_map(&map);
 *
 * Parameters:
 * none
 *
 * Return:
 * On success: a new non-null pointer to a dynamically allocated hashmap in
 *      which to store key/value object mappings.
 *      Use delete_mentry, get_mentry and set_mentry functions to manage 
 *      entries in the mapping. Use delete_map to free memory allocated to 
 *      the map.
 * On failure: NULL, and errno is set to ENOMEM if memory allocation 
 *      failed
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows:
 *      ENOMEM - not enough space: if dynamic allocation of the map fails
 */
omap* create_map();

/*
 * Function:
 * create_map_wbuckets(int nbuckets)
 * 
 * Description:
 * Creates an object hash map with the specified number of buckets.
 *
 * Usage: 
 *      omap* map = create_map_wbuckets(10); // creates map with 10 buckets
 *      ...                         
 *      ...
 *      delete_map(&map);
 *
 * Parameters:
 * nbuckets - the number of buckets to create in the hashmap. The number of 
 *      buckets determines the efficiency of the hashmap. The more buckets 
 *      there are the more likely that objects will be stored in  short lists,
 *      reducing the time taken to insert and retrieve objects. However, more
 *      buckets also means more memory is used for the hash map (and the 
 *      more likely it is that there is wasted space with slots for empty
 *      buckets).
 *
 * Return:
 * On success: a new non-null pointer to a dynamically allocated hashmap in
 *      which to store key/value object mappings.
 *      Use delete_mentry, get_mentry and set_mentry functions to manage 
 *      entries in the mapping. Use delete_map to free memory allocated to 
 *      the map.
 * On failure: NULL, and errno is set to EINVAL or ENOMEM as specified 
 *      under Errors.
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows:
 *      EINVAL - invalid argument:  if nbuckets is less than 1
 *      ENOMEM - not enough space: if dynamic allocation of the map fails
 */
omap* create_map_wbuckets(int nbuckets);

/*
 * Function:
 * delete_map(omap** m)
 * 
 * Description:
 * Delete an object hash map that was previously created using either 
 * create_map or create_map_wbuckets.
 *
 * Usage: 
 *      omap* map = create_map();
 *      ...
 *      ...
 *      delete_map(&map);
 *
 * Parameters:
 * map - the address of a pointer to a map, this function has no effect if 
 *      map is NULL
 *
 * Return:
 * No return value but a side effect of this function is that the pointer to 
 * the map is set to NULL. That is, map in the example code above will 
 * be NULL after the call to delete_map.
 *
 * Errors:
 * Not applicable
 */
void delete_map(omap** m);

/*
 * Function:
 * delete_mentry(omap* m, void* key)
 * 
 * Description:
 * Deletes the entry in the given map that corresponds to the given key.
 *
 * Usage:
 *      char* key = "my_key";           // note: it would be normal to 
 *      int val = 10;                   // dynamically allocate key and value
 *      omap* map = create_map();
 *      if (!set_mentry(map, key, &val)) 
 *      ...         // handle error
 *      ...
 *      int* deleted_val = (int*) delete_mentry(map, key);
 *      ...         // *deleted_val == val
 *      ...
 *      delete_map(&map);
 *          
 * Parameters:
 * map - the map to delete the entry from
 * key - the key for the entry to delete
 *
 * Return:
 * On success: the value that was stored in the map for the given key 
 * On failure: NULL if either of the map or key parameters is NULL or there 
 *      is no entry in the map for the given key, in which case errno is set 
 *      to EINVAL
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows:
 *      EINVAL - invalid argument: if either map or key is NULL or there is no
 *          entry in the map for the given key
 */
void* delete_mentry(omap* map, void* key);

/*
 * Function:
 * get_mentry(omap* map, void* key)
 * 
 * Description:
 * Gets the entry in the given map that corresponds to the given key.
 *
 * Usage:
 *      char* key = "my_key";           // note: it would be normal to 
 *      int val = 10;                   // dynamically allocate key and value
 *      omap* map = create_map();
 *      if (!set_mentry(map, key, &val)) 
 *          // handle error
 *      ...
 *      ...
 *      int* val_in_map = (int *) get_mentry(map, key);
 *      if (*val_in_map == val)
 *          printf("got the correct value: %d\n", *val_in_map);
 *      ...
 *      ...
 *      deleteMap(&map);
 *          
 * Parameters:
 * map - the map to get the entry from
 * key - the key for the entry to get
 *
 * Return:
 * On success: the value stored in the map for the given key
 * On failure: NULL if either the map or key parameters is NULL or there
 *      is no entry for the given key, in which case errno is set to EINVAL
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows:
 *      EINVAL - invalid argument: if either map or key is NULL, or there is no
 *          entry in the map for the given key
 */
void* get_mentry(omap* map, void* key);

/*
 * Function:
 * get_numbuckets(omap* map)
 * 
 * Description:
 * Gets the number of buckets in the map
 *
 * Usage:
 *      omap* map = create_map();
 *      int nbuckets = get_numbuckets(map); 
 *      printf("number of buckets is: %d\n", nbuckets); 
 *              // nbuckets will be OMAP_DEFAULT_NBUCKETS
 *      ...
 *      ...
 *      delete_map(&map);
 *          
 * Parameters:
 * map - the map to get the number of buckets for
 *
 * Return:
 * On success: a count of the number of buckets in the map
 * On failure: -1 if the map parameter is NULL and the errno is set to EINVAL
 *
 * Errors:
 * If the call fails, -1 will be returned and errno will be set as follows:
 *      EINVAL - invalid argument: if map is NULL
 */
int get_numbuckets(omap* map);

/*
 * Function:
 * get_numentries(omap* map)
 * 
 * Description:
 * Gets the number of entries (key/value pairs) in the map
 *
 * Usage:
 *      omap* map = create_map();
 *      int nentries = get_numentries(map); 
 *      printf("number of buckets is: %d\n", nentries);  
 *              // nentries will be 0
 *      char* key = "my_key";
 *      int val = 10;
 *      if (!set_mentry(map, key, &val)) 
 *          // handle error
 *      nentries = get_numentries(map)
 *      printf("number of buckets is: %d\n", nentries);  
 *              // nentries will now be 1
 *      ...
 *      ...
 *      delete_map(&map);
 *          
 * Parameters:
 * map - the map to get the number of entries for
 *
 * Return:
 * On success: a count of the number of key/value pairs in the map
 * On failure: -1 if the map parameter is NULL and the errno is set to EINVAL
 *
 * Errors:
 * If the call fails, -1 will be returned and errno will be set as follows:
 *      EINVAL - invalid argument: if map is NULL
 */
int get_numentries(omap* map);

/*
 * Function:
 * set_mentry(omap* map, void* key, void* val)
 * 
 * Description:
 * Sets the given key/value pair entry in the given map. If there is an 
 * existing entry for the given key, then the corresponding value will be 
 * overwritten with the given val. If there is no existing entry, 
 * then a new key/value pair entry will be created. 
 *
 * Usage:
 *      omap* map = create_map();
 *      char* key = "my_key";
 *      int val = 10;
 *      if (!set_mentry(map, key, &val)) 
 *          // handle error
 *      // the map now contains the entry key/&val
 *      ...
 *      ...
 *      delete_map(&map);
 *          
 * Parameters:
 * map - the map to set the given entry
 * key - the key for the entry
 * val - the value of the entry
 *
 * Return:
 * On success: true if the map is updated with the given key/value mapping
 * On failure: false if any of the map, key or val parameters is NULL or 
 *      if a new entry could not be allocated
 *
 * Errors:
 * If the call fails, false will be returned and errno will be set as follows:
 *      EINVAL - invalid argument: if any of map, key or val is NULL
 *      ENOMEM - not enough space: if a new entry could not be allocated
 */
bool set_mentry(omap* map, void* key, void* val);

#endif 