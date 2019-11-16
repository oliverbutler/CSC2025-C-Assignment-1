/*
 * Replace the following string of 0s with your student number
 * B8012181
 */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include "obj_store.h"

#define OSTR_REP_MAX 4096   /* max size of a string representation to write 
                             * to file
                             */

static bool ostore_on = false;  /* ostore enabled flag */

static char* OFILE_FMT = "./%s/%s/%#zx.txt"; 
                                        /* format for object store file path */
static char* OSTORE_DIR = "ostore";     /* name of object store directory */
static char* TYPEPATH_FMT = "%s/%s";    /* format for typedir path */

/* 
 * Declaration of private _create_ostore_dir helper function.
 * 
 * This function creates an object store directory if it does not already 
 * exist. 
 * If the typedir parameter is NULL then the top-level ostore directory 
 * will be created. 
 * If the typedir parameter is not NULL then a type director ostore/<typedir>
 * will be created (where typedir is int or str etc.).
 * If directory creation succeeds or the relevant directory already exists
 * the function returns true,  otherwise it returns false and errno will be 
 * set.
 */
bool _create_ostore_dir(const char* typedir);

/* 
 * Declaration of private _get_ofile_path helper function.
 * 
 * Given an object representation, this function returns
 * a newly allocated pathname for the object file in the store. 
 * OFILE_FMT defines the path format. 
 * For example, if the object_rep id is 0x7f91c1402710, the returned path 
 * will be:
 *      ./ostore/int/0x7f91c1402710.txt
 * 
 * assuming the object_rep type is "int"
 *
 * _get_ofile_path dynamically allocates the path name. It is the user's
 * responsibility to free the allocated memory.
 *
 * _get_ofile_path is used by store_obj to create the filename to use to 
 * store an object and by unlink_obj to create the filename to unlink/delete.
 */
char *_get_ofile_path(object_rep* obj_rep);

/* enable_ostore: implemented, do NOT change */
bool enable_ostore() {
    ostore_on = _create_ostore_dir(NULL);
    
    return ostore_on;
}

/* ostore_is_on: implemented, do NOT change */
bool ostore_is_on() {
    return ostore_on;
}

/* 
 * TODO: IMPLEMENT store_obj
 * See comments to the store_obj function declaration in obj_store.h for the
 * specification of this function. Also see the comments below in the body of
 * the function.
 */
bool store_obj(object_rep* obj_rep) {
  /* 
  * You should:
  *      - include appropriate error checking of the parameter to the 
  *        function and its fields and of return values from library 
  *        and helper function calls
  *      - only use file system operations specified in the rules in
  *        README.txt
  *      - use helper functions to create a type subdirectory and get 
  *        the get the path of the file to write to
  *      - write the valstr of the object representation to the file
  *      - free any memory you were responsible for allocating
  *      - return true if the string representation is successfully written 
  *        to file and false otherwise, in which case no file should remain
  */
  if(!obj_rep || !obj_rep->valstr) {
    errno = EINVAL;
    return false;
  }
  bool dir = _create_ostore_dir(obj_rep->type);
  if(dir && ostore_is_on()) {
    char* path = _get_ofile_path(obj_rep);
    if(!path) return false;
    int fo = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(!fo) return false;
    int fw = write(fo, obj_rep->valstr, strlen(obj_rep->valstr));
    if(!fw) return false;
    if(close(fo) < 0) return false;
    return true;
  }
  errno = ENOENT;
  return false;
}

/* unlink_obj: implemented, do NOT change */
void unlink_obj(object_rep* obj_rep) {
    if (ostore_on) {
        char *ofile_path = _get_ofile_path(obj_rep); 
                                // returns NULL for bad obj_rep
    
        if (ofile_path) {
            unlink(ofile_path);
        
            free(ofile_path);
        }
    }
    
    return;
}

/* _create_ostore_dir: see specification at start of this file. 
 * Do NOT change this function.
 */
bool _create_ostore_dir(const char* typedir) {
    struct stat sbuf;
    
    char* typepath = NULL;
    char* path = OSTORE_DIR;
    
    if (typedir) {
        asprintf(&typepath, TYPEPATH_FMT, OSTORE_DIR, typedir);  
        path = typepath;
    }  
    
    errno = 0;
    
    int r = stat(path, &sbuf);
    
    if (r) {
        if (errno == ENOENT) {
            errno = 0;
            /* ostore doesn't exist, create it */
            r = mkdir(path, 0755);
        }
    } else if (!S_ISDIR(sbuf.st_mode)) {
        /* exists but is not a directory */
        r = -1;
        errno = ENOTDIR;
    }
    
    free(typepath);

    return r == 0;
}

/* _get_ofile_path: see specification at start of this file. 
 * Do NOT change this function.
 */
char *_get_ofile_path(object_rep* obj_rep) {
    char *ofile_path = NULL;

    if (obj_rep && obj_rep->type) 
        (void) asprintf(&ofile_path, OFILE_FMT, OSTORE_DIR, obj_rep->type,
            obj_rep->id);
    
    return ofile_path;
}
