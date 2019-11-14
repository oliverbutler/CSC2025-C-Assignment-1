/*
 * Replace the following string of 0s with your student number
 * b8012181
 */
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#include "obj_map.h"
#include "obj_store.h"
#include "integer.h"

/* 
 * Internal object management functions to create, delete and 
 * provide string representation of the underlying integer value.
 */ 
 
/* The private _object_map that maps an Integer object to its int value
 * Do NOT change this declaration.
 */
static omap* _object_map = NULL;

/*
 * Private _new_intobj function to dynamically allocate a new int value and 
 * store it in the internal object_map with the address of the Integer object
 * as the key to the newly allocated int value. This means that there is no 
 * direct access to the value of an Integer, which can only be manipulated
 * and obtained using the Integer member functions.
 *
 * Do NOT change this function.
 */
static int* _new_intobj(Integer self, int value) {
    int* val = NULL;

    if (_object_map || (_object_map = create_map())) {
        val = (int*) malloc(sizeof(int));
    
        if (val) {
            *val = value;
            if (!set_mentry(_object_map, self, val)) {
                free(val);
                val = NULL;
            }
        }
    }
    
    return val;
}

/* The string representation of an Integer for saving to file.
 * Do NOT change these declarations.
 */
static const char* STR_REP_FMT = "%d\n"; 
static const char* TYPE_STR = "int";

/*
 * Private _delete_int function deletes an integer object and its internal
 * object-to-value mapping, freeing all dynamically allocated memory and 
 * storage associated with the object.
 *
 * Do NOT change this function.
 */
static void _delete_int(Integer* ai, bool check_store) {
    if (ai && *ai) {
        if (check_store && ostore_is_on()) {
            object_rep obj_rep = {TYPE_STR, (uintptr_t) *ai, NULL};
            unlink_obj(&obj_rep);
        }
        
        int* val = delete_mentry(_object_map, *ai);
        if (val)
            free(val);

        memset(*ai, 0, sizeof(struct integer)); 
                // 0s integer memory in case reused

        free(*ai);
        *ai = NULL;
    }
}

/* 
 * Private _store_obj_rep function.
 * This function does nothing if the object store is not enabled (and 
 * returns true for success of the function). If the object store is enabled
 * the function will attempt to store a string representation of the given
 * integer to the object store. The string representation is the int value
 * followed by a new line (see STR_REP_FMT).
 * Do NOT change this function.
 */
static bool _store_obj_rep(Integer oi, int val) {
    if (!ostore_is_on()) 
        return true;
    
    bool r = false;
    char* valstr = NULL;    
    (void) asprintf(&valstr, STR_REP_FMT, val);
    
    if (valstr) {
        object_rep obj_rep = { TYPE_STR, (uintptr_t) oi, valstr };
        r = store_obj(&obj_rep);
        
        free(valstr);
    }
    
    return r;
}

/*
 * Helper functions to do addition and check whether can divide.
 */
 
/*
 * Private _add_vals helper function provides bounds checking addition of 
 * two int values. 
 * This function will return a new Integer whose value is the result of 
 * the addition of the parameters to the function if that addition would 
 * not cause overflow. Otherwise it sets errno to ERANGE and returns NULL.
 *
 * Do NOT change this function.
 */
static Integer _add_vals(int lhs, int rhs) {
    Integer r = NULL;
    
    if (lhs >= 0) {
        if (rhs <= 0 || (lhs <= INT_MAX - rhs)) {
            r = newInteger(lhs + rhs);
        }
    } else if (rhs >= 0) {
        r = newInteger(lhs + rhs);
    } else if (lhs >= -INT_MAX - (rhs + 1)) {
        /* note: -INT_MAX - (rhs + 1) == INT_MIN - rhs but guards 
         * against rhs == INT_MIN */
        r = newInteger(lhs + rhs);
    }
    
    if (!r) errno = ERANGE;
    
    return r;
}

/*
 * Private _can_divide helper function checks whether division of num by 
 * den would succeed.
 * This function returns true if the division of its num parameter by its 
 * den parameter would not cause overflow. Otherwise it sets errno to ERANGE
 * and returns false.
 *
 * Do NOT change this function.
 */
static bool _can_divide(int num, int den) {
    if (!den || (num == INT_MIN && den == -1)) {
        errno = ERANGE;
        return false;
    }
    
    return true;
}

/* 
 * Prototype of private _add function for implementation of the add member of
 * struct integer
 */
static Integer _add(Integer self, Integer i); 
/* 
 * Prototype of private _subtract function for implementation of the subtract
 * member of struct integer
 */
static Integer _subtract(Integer self, Integer i);
/* 
 * Prototype of private _multiply function for implementation of the multiply
 * member of struct integer
 */
static Integer _multiply(Integer self, Integer i);
/* 
 * Prototype of private _divide function for implementation of the divide 
 * member of struct integer
 */
static Integer _divide(Integer self, Integer i);
/* 
 * Prototype of private _modulo function for implementation of the modulo 
 * member of struct integer
 */
static Integer _modulo(Integer self, Integer i);
/* 
 * Prototype of private _get_value function for implementation of the get_value     
 * member of struct integer
 */
static int _get_value(Integer self);

/*
 * Implementation of public functions/methods.
 * See integer.h for specification of the following functions.
 */
 
/* newInteger: implemented, do NOT change */
Integer newInteger(int value) {
    Integer self = (Integer) malloc(sizeof(struct integer));
   
    if (self) {
        int* val = _new_intobj(self, value);

        if (val) {
            self->add = _add;
            self->subtract = _subtract;
            self->multiply = _multiply;
            self->divide = _divide;
            self->modulo = _modulo;
            self->get_value = _get_value;
        
            if (!_store_obj_rep(self, *val))
                _delete_int(&self, false); // ostore on but storage failed
        } else {
            free(self);
            self = NULL;
        }
    }

    return self;
}

/* deleteInteger: implemented, do NOT change */
void deleteInteger(Integer* ai) {    
    _delete_int(ai, true);
}

/* printInteger: implemented, do NOT change */
int printInteger(const char* format, Integer i) {
    return fprintInteger(stdout, format, i);
}

/* fprintInteger: implemented, do NOT change */
int fprintInteger(FILE* stream, const char* format, Integer i) {
    int* io = (int*) get_mentry(_object_map, i);

    return io ? fprintf(stream, format, *io) : -1;
}

/* 
 * TODO: IMPLEMENT _add
 * see comments to the add member of struct integer in integer.h for the
 * specification of this function
 */
Integer _add(Integer self, Integer i) {
  int* so = (int*) get_mentry(_object_map, self);
  int* io = (int*) get_mentry(_object_map, i);
  Integer r = NULL;

  if (so && io) {
    int sv = *so;
    int iv = *io;
    bool can_add = false;
    if (sv >= 0) can_add = iv <= INT_MAX - sv;
    else can_add = iv >= INT_MIN - sv;

    if (can_add) r = _add_vals(sv, iv);
    else errno = ERANGE;
  }
  return r;
}

/* 
 * TODO: IMPLEMENT _subtract
 * see comments to the subtract member of struct integer in integer.h for the
 * specification of this function
 */
Integer _subtract(Integer self, Integer i) {
  int* so = (int*) get_mentry(_object_map, self);
  int* io = (int*) get_mentry(_object_map, i);
  Integer r = NULL;

  if (so && io) {
    int sv = *so;
    int iv = *io;
    bool can_subtract = true;
    if ((iv < 0) && (sv > INT_MAX + iv)) can_subtract = false;
    if ((iv > 0) && (sv < INT_MIN + iv)) can_subtract = false;

    if (can_subtract) r = newInteger(sv - iv);
    else errno = ERANGE;
  }
  return r;
}

/* _multiply: implemented, do NOT change */
Integer _multiply(Integer self, Integer i) {
    int* so = (int*) get_mentry(_object_map, self);
    int* io = (int*) get_mentry(_object_map, i);
    Integer r = NULL;

    if (so && io) {
        int sv = *so;
        int iv = *io;
        bool can_multiply = false;
    
        if (!sv || !iv) {
            can_multiply = true;
        } else if (sv > 0) {
            can_multiply = iv > 0 ? sv <= INT_MAX / iv : iv >= INT_MIN / sv;
        } else {
            can_multiply = iv > 0 ? sv >= INT_MIN / iv : sv >= INT_MAX / iv;
        }
    
        if (can_multiply) {
            r = newInteger(sv * iv);
        } else {
            errno = ERANGE;
        }
    }
    
    return r;
}

/* 
 * TODO: IMPLEMENT _divide 
 * see comments to the divide member of struct integer in integer.h for the
 * specification of this function
 */
Integer _divide(Integer self, Integer i) {
  int* so = (int*) get_mentry(_object_map, self);
  int* io = (int*) get_mentry(_object_map, i);
  Integer r = NULL;

  if (so && io) {
    int sv = *so;
    int iv = *io;

    if (_can_divide(sv, iv)) r = newInteger(sv / iv);
    else errno = ERANGE;
  }
  return r;
}

/* 
 * TODO: IMPLEMENT _modulo
 * see comments to the modulo member of struct integer in integer.h for the
 * specification of this function
 */
Integer _modulo(Integer self, Integer i) {
  int* so = (int*) get_mentry(_object_map, self);
  int* io = (int*) get_mentry(_object_map, i);
  Integer r = NULL;

  if (so && io) {
    int sv = *so;
    int iv = *io;

    if ((iv == 0) || ((sv == INT_MIN) && iv == -1))
      errno = ERANGE;
    else r = newInteger(sv % iv);
  }
  return r;
}

/* _get_value: implemented, do NOT change */
int _get_value(Integer self)  {
    int* so = (int*) get_mentry(_object_map, self);

    return so ? *so : 0;
}


