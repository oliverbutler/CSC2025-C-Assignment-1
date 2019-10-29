#ifndef _INTEGER_H
#define _INTEGER_H
#include <stdbool.h>

/*
 * Type definition:
 * Integer
 * 
 * Description:
 * Declares Integer to be an alias for the type: "pointer to a struct integer".
 * That is, Integer can be used as a type name in place of "struct integer*".
 * (See below for definition of struct integer).
 */
typedef struct integer* Integer;

/*
 * Function:
 * newInteger(int value)
 * 
 * Description:
 * Dynamically allocate a new struct integer that will be associated with 
 * the given value and return a pointer (Integer) to the allocated struct.  
 * Integer values are maintained in an object map and are not directly 
 * accessible via the struct.
 * If the object store is enabled, the new integer will be saved to persistent
 * storage as part of successful allocation.
 * It is the user's responsibility to use deleteInteger to subsequently free
 * the allocated memory.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *      ...
 *      ...
 *      deleteInteger(&i);
 *
 * Parameters:
 * value - the int value to associate with the new struct integer
 *
 * Return:
 * On success: a new non-null pointer to a dynamically allocated struct 
 *      integer (or Integer) with the given "internal" value 
 * On failure: NULL, and errno will be set to ENOMEM or to a value indicating
 *      failure to store the new struct integer if the object store is enabled  
 *      and storage fails (see store_obj in obj_store.h)
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows.
 *      ENOMEM - not enough space: if dynamic allocation fails
 *      Other errno values set by store_obj (see obj_store.h)
 */
Integer newInteger(int value);

/*
 * Function:
 * deleteInteger(int* ai)
 * 
 * Description:
 * Delete a struct integer previously allocated by the newInteger function.
 * If the object store is enabled the integer will be deleted from persistent
 * storage as part of successful deletion.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *      ...
 *      ...
 *      deleteInteger(&i);
 *      // i is now NULL
 *
 * Parameters:
 * ai - the address of an Integer pointer
 *
 * Return:
 * No return value but a side effect of this function is that the pointer to 
 * the struct integer is set to NULL. That is, i in the example above will 
 * be NULL after the call to deleteInteger.
 *
 * Errors:
 * Not applicable
 */
void deleteInteger(Integer* ai);

/*
 * Function:
 * printInteger(const char* format, Integer i)
 * 
 * Description:
 * A wrapper for the C library printf function (see printf(3) man page).
 * Prints the given Integer i to stdout according to the given format.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *      printInteger("Integer i: %d\n", i);  // prints "Integer i: 10\n" to 
 *      ...                                  // stdout
 *      ...
 *      deleteInteger(&i);
 *
 * Parameters:
 * format - the format of output with %d replaced by integer i (see printf(3) 
 *          for details)
 * i - the integer to print
 *
 * Return:
 * The number of characters printed not including the terminating '\0'
 *
 * Errors:
 * See printf(3) man page.
 */
int printInteger(const char* format, Integer i);

/*
 * Function:
 * fprintInteger(FILE* stream, const char* format, Integer i)
 * 
 * Description:
 * A wrapper for the C library fprintf function (see fprintf(3) man page).
 * Prints the given Integer i to the given open file stream according to the
 * given format.
 *
 * Usage: 
 *      Integer i = newInteger(10);
 *      fprintInteger(stderr, "Integer i: %d\n", i); 
 *      ...                         // prints "Integer i: 10\n" to stderr
 *      ...
 *      deleteString(&i);
 *
 * Parameters:
 * stream - the open file stream to print the integer to
 * format - the format of output with %d replaced by integer i (see fprintf(3) 
 *          for details)
 * i - the integer to print
 *
 * Return:
 * The number of characters printed not including the terminating '\0'
 *
 * Errors:
 * See fprintf(3) man page.
 */
int fprintInteger(FILE* stream, const char* format, Integer i);

/*
 * Type definition:
 * struct integer - an integer with arithmetic operations that detect and signal
 * positive or negative overflow errors.
 * 
 * Description:
 * The definition of an integer type with "member functions":
 *      add
 *      subtract
 *      multiply
 *      divide
 *      modulo
 *      get_value
 * 
 * The "member functions" are pointers to functions that are defined in 
 * integer.c
 * The pointer assignments are made by the newString function.
 *
 * The member functions operate on values stored in an object map (see
 * obj_map.h) and there is no direct access to the value associated with a
 * struct integer.
 * 
 * Integer (defined above) is another name for a pointer to a struct integer.
 */
struct integer {
    /*
     * Pointer to function member field:
     * add(Integer self, Integer i)
     * 
     * Description:
     * Add the values of two Integers and return a new Integer that is the 
     * result.
     * 
     * Usage: 
     *      Integer r = lhs->add(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which add is called (e.g. lhs in above
     *      example). 
     * i - the non-null Integer to add to self
     *
     * Return:
     * On success: a new non-null Integer whose value is the sum of the values
     *      of self and i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) + i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*add)(Integer self, Integer i);

    /*
     * Pointer to function member field:
     * subtract(Integer self, Integer i)
     * 
     * Description:
     * Subtract the value of one Integer from another and return a new Integer
     * that is the result.
     * 
     * Usage: 
     *      Integer r = lhs->subtract(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which subtract is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to subtract from self
     *
     * Return:
     * On success: a new non-null Integer whose value is the subtraction of 
     *      the value of i from the value of self
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) - i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*subtract)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * multiply(Integer self, Integer i)
     * 
     * Description:
     * Multiply the values of two Integers and return a new Integer that is the 
     * result.
     * 
     * Usage: 
     *      Integer r = lhs->multiply(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which multiply is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to multiply self with
     *
     * Return:
     * On success: a new non-null Integer whose value is the multiplication of 
     *      the value of self with the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) * i->get_value(i) would cause positive 
     *          or negative integer overflow
     */
    Integer (*multiply)(Integer self, Integer i);

    /*
     * Pointer to function member field:
     * divide(Integer self, Integer i)
     * 
     * Description:
     * Divide the value of one Integer by another and return a new Integer that
     * is the result.
     * 
     * Usage: 
     *      Integer r = lhs->divide(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which divide is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to divide self by
     *
     * Return:
     * On success: a new non-null Integer whose value is the integer division 
     *      of the value of self by the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) / i->get_value(i) would cause positive 
     *          or negative integer overflow or division by zero. 
     */
    Integer (*divide)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * modulo(Integer self, Integer i)
     * 
     * Description:
     * Divide the value of one Integer by another and return a new Integer that
     * is the remainder after integer division (the modulus or result of the 
     * modulo operation)
     * 
     * Usage: 
     *      Integer r = lhs->modulo(lhs, rhs);
     *                                  // assume lhs and rhs have been created
     *                                  // with newInteger and are not null
     *
     * Parameters:
     * self - the non-null Integer on which modulo is called (e.g. lhs in
     *      above example). 
     * i - the non-null Integer to divide self by to compute the modulus
     *
     * Return:
     * On success: a new non-null Integer whose value is the modulus of 
     *      the value of self divided by the value of i
     * On failure: NULL, and errno will be set to EINVAL or ERANGE
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set as follows.
     *      EINVAL - invalid argument: if either self or i is NULL
     *      ERANGE - result too large: if the result of the operation 
     *          self->get_value(self) % i->get_value(i) would cause positive 
     *          or negative integer overflow or division by zero. 
     */
    Integer (*modulo)(Integer self, Integer i);
    
    /*
     * Pointer to function member field:
     * get_value(Integer self)
     *
     * Description:
     * Get the primitive int value of the struct integer.
     * 
     * Usage: 
     *      int i = oi->get_value(oi);
     *                                  // assume oi has been created
     *                                  // with newInteger and is not null
     *
     * Parameters:
     * self - the non-null Integer on which get_value is called (e.g. oi in
     *      above example). 
     *
     * Return:
     * On success: the value of the integer struct or 0 if self is NULL, and
     *      errno will be set to EINVAL to distinguish this with a valid value
     *      of 0.
     *
     * Errors:
     * If self is NULL, 0 will be returned and errno will be set to EINVAL
     */
    int (*get_value)(Integer self);
};

#endif