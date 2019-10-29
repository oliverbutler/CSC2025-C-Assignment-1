#ifndef _OBJ_STORE_H
#define _OBJ_STORE_H
#include <stdbool.h>
#include <stdint.h>

/* 
 * Declaration of the object_rep type to represent the key information about
 * an object in a persistent object store - the type of the object, its unique 
 * identifier (typically the memory address) and the string representation of 
 * the object.
 */
typedef struct object_rep {
    const char* type;   /* type of the object e.g. int or str */
    uintptr_t id;       /* identifier (numeric address) of object */
    const char* valstr; /* string representation of object state */
} object_rep;

/*
 * Function:
 * enable_ostore()
 * 
 * Description:
 * Enables storage of objects to a file-based object store and, if necessary,
 * creates the directory to store the objects.
 *
 * Usage: 
 *      bool r = enable_ostore();
 *
 * Parameters:
 * none, the object store directory will be a sub-directory of the current 
 * directory called ostore.
 *
 * Return:
 * true if the ostore directory already exists or if it was successfully 
 * created, false otherwise, in which case errno will be set. 
 * If the result of this function is true, ostore_is_on will return true.
 *
 * Errors:
 * If the call fails, the function returns false and ostore will not exist
 * and ostore_is_on will return false and errno will be set to a value that
 * indicates why ostore could not be created.
 */
bool enable_ostore();

/*
 * Function:
 * ostore_is_on()
 * 
 * Description:
 * Indicates whether the object store is on/enabled and, therefore, whether 
 * objects can be written to the ostore directory.
 *
 * Usage: 
 *      bool r = ostore_is_on();
 *
 * Parameters:
 * none
 *
 * Return:
 * true if the object store is enabled (i.e. the ostore directory has been 
 * created by a successful call to enable_ostore), 
 * false otherwise (i.e. enable_ostore has not been called or disable_ostore 
 * has been called).
 *
 * Errors:
 * Not applicable
 */
bool ostore_is_on();

/*
 * Function:
 * store_obj(object_rep* obj_rep)
 * 
 * Description:
 * Writes the value string representation (obj_rep->valstr) of the given 
 * object_rep to a text file whose name is obj_rep->id plus the ".txt" 
 * extension in the obj_rep->type sub-directory of the ostore. 
 * 
 * An example layout of int and string objects in the object store is:
 *
 *      ostore/
 *          int/
 *              0x8053968.txt
 *              0x8053c18.txt
 *          str/
 *              0x8051e10.txt
 *              0x8052148.txt
 *
 * where int/0x8053968.txt and int/0x8053c18.txt are files containing integer
 * values and str/0x8051e10.txt and str/0x8052148.txt are files containing
 * strings. The data/representation written to the files is valstr field of the 
 * relevant object_rep struct.
 *
 * Usage: 
 *      bool r = store_obj(obj_rep);
 *                      // see newInteger in integer.c and newString in
 *                      // string_o.c and tests in test_obj_store.c for 
 *                      // examples of usage 
 *
 * Parameters:
 * obj_rep - a pointer to the object_rep representation of the object to 
 *      store (its type, id and string representation value)
 *
 * Return:
 * true if the object store is enabled and the object is stored successfully, 
 * false otherwise. 
 *
 * Errors:
 * If the call fails, false will be returned and errno will be set as follows.
 *      EINVAL - invalid argument: if obj_rep is NULL or obj_rep->valstr is
 *          NULL
 *      ENOENT - no such entity: if the object store is not enabled or the
 *          ostore directory does not exist
 *      Other errno values related to I/O errors writing to file.
 */
bool store_obj(object_rep* obj_rep);

/*
 * Function:
 * unlink_obj(object_rep* obj_rep)
 * 
 * Description:
 * Unlinks/deletes the file for the given object representation from the 
 * object store. Note: only type and id fields of obj_rep are used. The 
 * valstr can be NULL and is ignored. 
 *
 * Usage: 
 *      unlink_obj(obj_rep);
 *
 * Parameters:
 * obj_rep - a pointed to the object_rep representation of the object to
 *      be removed from the store
 *
 * Return:
 * Not applicable
 *
 * Errors:
 * Not applicable. If obj is non-NULL and there is a file in ostore that
 * corresponds to the given object, the file will be deleted.
 */
void unlink_obj(object_rep* obj_rep);

#endif