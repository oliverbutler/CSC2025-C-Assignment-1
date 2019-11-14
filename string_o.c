/*
 * Replace the following string of 0s with your student number
 * B8012181
 */
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#include "obj_map.h"
#include "obj_store.h"
#include "string_o.h"

/* 
 * Internal value object management functions to create, delete and 
 * provide string representation of the underlying string value.
 */ 

/* The private _object_map that maps an String object to its char* value and
 * length.
 * Do NOT change this declaration.
 */
static omap* _object_map = NULL;

/*
 * struct definition for internal representation of a string value.
 * Do NOT change this declaration.
 */
typedef struct strobj {
    int len;        /* the length of a string value */
    char* val;      /* the value of a string */
} strobj;

/*
 * Private _new_strobj function to dynamically allocate a new string value and 
 * store it in the internal object_map with the address of the String object
 * as the key to the newly allocated value. This means that there is no 
 * direct access to the value of a String, which can only be manipulated
 * and obtained using the String member functions.
 *
 * Do NOT change this function.
 */
static strobj* _new_strobj(String self, const char* value) {
    strobj* sobj = NULL;
    
    if (_object_map || (_object_map = create_map())) {
        int len = strnlen(value, STR_LEN_MAX);
    
        char* val = strndup(value, len);
    
        if (val) {
            sobj = (strobj*) malloc(sizeof(strobj));
        
            if (sobj && set_mentry(_object_map, self, sobj)) {
                sobj->len = len;
                sobj->val = val;
            } else {
                free(val);
                free(sobj);
                sobj = NULL;
            }
        }
    }
    
    return sobj;
}

/* The string representation of type and value for saving to file.
 * Do NOT change these declarations.
 */
static const char* STR_REP_FMT = "%d:%s\n"; /* strobj->len:strobj->val */
static const char* TYPE_STR = "str";

/*
 * Private _delete_str function deletes a string object and its internal
 * object-to-value mapping, freeing all dynamically allocated memory and 
 * storage associated with the object.
 * Do NOT change this function.
 */
void _delete_str(String* as, bool check_store) {
    if (as && *as) {
        if (check_store && ostore_is_on()) {
            object_rep obj_rep = {TYPE_STR, (uintptr_t) *as, NULL};
            unlink_obj(&obj_rep);
        }
        
        strobj* sobj = delete_mentry(_object_map, *as);
        
        if (sobj) {
            free(sobj->val);
            free(sobj);
        }
        
        memset(*as, 0, sizeof(struct string));
            // 0s string memory in case reused
            
        free(*as);
        *as = NULL;
    }
}

/* 
 * Private _store_obj_rep function.
 * This function does nothing if the object store is not enabled (and 
 * returns true for success of the function). If the object store is enabled
 * the function will attempt to store a string representation of the given
 * String to the object store. The string representation is the length of 
 * the string followed by a colon followed by the string value followed by a 
 * new line (see STR_REP_FMT).
 * Do NOT change this function.
 */
static bool _store_obj_rep(String oi, strobj* sobj) {
    if (!ostore_is_on())
        return true;
        
    bool r = false;
    char* valstr = NULL;    
    (void) asprintf(&valstr, STR_REP_FMT, sobj->len, sobj->val);
    
    if (valstr) {
        object_rep obj_rep = { TYPE_STR, (uintptr_t) oi, valstr };
        r = store_obj(&obj_rep);
        
        free(valstr);
    }
    
    return r;
}

/* 
 * Prototype of private _char_at function for implementation of the char_at 
 * member of struct string
 */
static char _char_at(String self, int posn);            
/* 
 * Prototype of private _concat function for implementation of the concat 
 * member of struct string
 */
static String _concat(String self, String s);     
/* 
 * Prototype of private _equals function for implementation of the equals 
 * member of struct string
 */
static bool _equals(String self, String s);             
/* 
 * Prototype of private _get_value function for implementation of the get_value 
 * member of struct string
 */
static char* _get_value(String self, char* buf);
/* 
 * Prototype of private _index_of function for implementation of the index_of 
 * member of struct string
 */
static int _index_of(String self, char c, int start);   
/* 
 * Prototype of private _length function for implementation of the length 
 * member of struct string
 */
static int _length(String self);                        
/* 
 * Prototype of private _split function for implementation of the split 
 * member of struct string
 */
static String* _split(String self, String delim); 
/* 
 * Prototype of private _substring function for implementation of the substring 
 * member of struct string
 */
static String _substring(String self, int start, int length);     

/*
 * Implementation of public functions/methods.
 * See string_o.h for specification of the following functions.
 */

/* newString: implemented, do NOT change */
String newString(const char* value) {
    if (!value) {
        errno = EINVAL;
        return NULL;
    }
    
    String self = (String) malloc(sizeof(struct string));
   
    if (self) {
        strobj* sobj = _new_strobj(self, value);
        
        if (sobj) {
            self->concat = _concat; 
            self->char_at = _char_at;
            self->equals = _equals;
            self->get_value = _get_value;
            self->index_of = _index_of;
            self->length = _length;
            self->split = _split;
            self->substring = _substring;
        
            if (!_store_obj_rep(self, sobj))
                _delete_str(&self, false); // ostore on but storage failed
        } else {
            free(self);
            self = NULL;
        }
    }

    return self;
}

/* deleteString: implemented, do NOT change */
void deleteString(String *as) { 
    _delete_str(as, true);
}

/* printString: implemented, do NOT change */
int printString(const char* format, String s) {
    return fprintString(stdout, format, s);
}

/* fprintString: implemented, do NOT change */
int fprintString(FILE* stream, const char* format, String s) {
    strobj* sobj = (strobj*) get_mentry(_object_map, s);
    
    return sobj ? fprintf(stream, format, sobj->val) : -1;
}

/* 
 * TODO: IMPLEMENT _char_at NOTE: Use strn* not str* because safety of operations
 * see comments to the char_at member of struct string in string_o.h for the
 * specification of this function
 */
char _char_at(String self, int posn) {
  strobj* sobj = (strobj*) get_mentry(_object_map, self);
  String r = 0;

  if(sobj) {
    char* str = sobj->val;
    if (posn >= 0 && posn < sobj->len) {
      r = str[posn];
    } else if(sobj->len == 0 && posn == 0) {
      r = 0;
    } else {
      errno = EINVAL;
    }
  }
  return r;
}

/* 
 * TODO: IMPLEMENT _concat
 * see comments to the concat member of struct string in string_o.h for the
 * specification of this function
 */
String _concat(String self, String s) {
  strobj* sobj = (strobj*) get_mentry(_object_map, self);
  strobj* sobjsrc = (strobj*) get_mentry(_object_map, s);
  String r = 0;
  if (sobj && sobjsrc) {
    char* concat = calloc(1, sobj->len + sobjsrc->len + 1); // malloc not work because random and overwriting shit

    if(!concat) {
      return NULL;
    }

    strncpy(concat, sobj->val, sobj->len);
    strncat(concat, sobjsrc->val, sobj->len + sobjsrc->len + 1);

    char temp[sobj->len + sobjsrc->len + 1];
    strncpy(temp, concat, sobj->len + sobjsrc->len + 1);
    free(concat);

    r = newString((const char*)temp);
  }
  return r;
}

/* 
 * TODO: IMPLEMENT _equals
 * see comments to the equals member of struct string in string_o.h for the
 * specification of this function
 */
bool _equals(String self, String s) {
  strobj* sobj = (strobj*) get_mentry(_object_map, self);
  strobj* sobj2 = (strobj*) get_mentry(_object_map, s);

  if (!sobj || !sobj2)
    return NULL;

  if(strcmp(sobj->val, sobj2->val) == 0)
    return true;
  else
    return false;
}

/* _get_value: implemented, do NOT change */
char* _get_value(String self, char* buf)  {
    strobj* sobj = (strobj*) get_mentry(_object_map, self);
    
    if (!sobj)
        return NULL;
        
    if (buf) {
        (void) strncpy(buf, sobj->val, sobj->len);
        buf[sobj->len] = '\0';
    } else {
        buf = strndup(sobj->val, sobj->len);
    }
    
    return buf;
}

/* 
 * TODO: IMPLEMENT _index_of
 * see comments to the index_of member of struct string in string_o.h for the
 * specification of this function
 */
int _index_of(String self, char c, int start) {
  strobj* sobj = (strobj*) get_mentry(_object_map, self);
  if(sobj && start >= 0 && start < sobj->len) {
    for(int i = start; i < sobj->len + 1; i ++)
      if(sobj->val[i] == c) 
        return i;
    return -1;
  } else {
    errno = EINVAL;
    return -2;
  }
}

/* _length: implemented, do NOT change */
int _length(String self) {
    strobj* sobj = (strobj*) get_mentry(_object_map, self);
    
    return sobj ? sobj->len : -1;
}

/* 
 * TODO: IMPLEMENT _split
 * see comments to the split member of struct string in string_o.h for the
 * specification of this function
 */
String* _split(String self, String delim) {
  strobj* sobj = (strobj*) get_mentry(_object_map, self);
  strobj* sobj_delim = (strobj*) get_mentry(_object_map, delim);

  if(sobj && sobj_delim) {
    String* splits = (String) malloc(sizeof(sobj));

    char* string_val = _get_value(self, NULL);

    char* token;
    int i = 0;
    while((token = strsep(&string_val, sobj_delim->val)) != NULL)
      splits[i++] = newString(token);
    splits[i] = '\0';
    
    return splits;

  } else {
    errno = EINVAL;
    return NULL;
  }
}

/* 
 * TODO: IMPLEMENT _substring
 * see comments to the substring member of struct string in string_o.h for the
 * specification of this function
 */
String _substring(String self, int start, int length) {
    return NULL;
}

/*  
 * Access to string value to simplify tests - this would be removed in 
 * production release of String. Do NOT call this function in any of the 
 * other functions of this file.
 * Do NOT change.
 */
const char* _test_string_val(String s) {
    strobj* sobj = (strobj*) get_mentry(_object_map, s);
    
    return sobj ? sobj->val : NULL;
}

