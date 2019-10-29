#ifndef _STRING_O_H
#define _STRING_O_H
#include <stdbool.h>
#include <stdio.h>

/* 
 * The maximum length of a string. Newly allocated strings are truncated to 
 * this length. The maximum memory allocation for a string will therefore be
 * STR_LEN_MAX + 1
 */
#define STR_LEN_MAX 1023

/*
 * Type definition:
 * String
 * 
 * Description:
 * Declares String to be an alias for the type: "pointer to a struct string".
 * That is, String can be used as a type name in place of "struct string*".
 * (See below for definition of struct string).
 */
typedef struct string* String;

/*
 * Function:
 * newString(const char* value)
 * 
 * Description:
 * Dynamically allocate a new struct string with an associated new internal 
 * char* value that represents the same string as the specified value parameter
 * and return a pointer to the allocated struct (a String object).  
 * newString dynamically allocates new memory for its internal char*. The
 * internal value will have a maximum string length of STR_LEN_MAX. 
 * This means that the internal value may be a truncation of 
 * the parameter value passed to newString.
 * String values are maintained in an object map and are not directly 
 * accessible via the struct.
 * If the object store is enabled, the new string will be saved to persistent
 * storage as part of successful allocation.
 * It is the user's responsibility to use deleteString to free memory allocated
 * by newString.
 *
 * Usage: 
 *      String i = newString("hello");
 *      ...
 *      ...
 *      deleteString(&i);
 *
 * Parameters:
 * value - the char* value to associate with the newly allocated struct string. 
 *      Note: if the length of parameter value is greater than STR_LEN_MAX 
 *      the internal value of the struct string will be a truncation to
 *      length STR_LEN_MAX.
 *
 * Return:
 * On success: a new non-null, pointer to a dynamically allocated string  
 *      struct (or String) with an internal dynamically allocated value 
 *      that is equivalent to the value given (or its truncation to STR_LEN_MAX)
 * On failure: NULL, and errno will be set to EINVAL or ENOMEM or to a value 
 *      set by the object store if it is enabled and storage fails
 *      (see store_obj in obj_store.h)
 *
 * Errors:
 * If the call fails, the NULL pointer will be returned and errno will be 
 * set as follows.
 *      EINVAL -  invalid argument: if value is NULL or string length is 
 *          greater than max length
 *      ENOMEM - not enough space: if dynamic allocation fails
 *      Other errno values set by store_obj and defined in obj_store.h
 */
String newString(const char* value);

/*
 * Function:
 * deleteString(String* as)
 * 
 * Description:
 * Delete a struct string previously allocated by the newString function.
 * deleteString also frees memory allocated for the internal char* value.
 * If the object store is enabled the string will be deleted from persistent
 * storage as part of successful deletion.
 *
 * Usage: 
 *      String s = newString("hello");
 *      ...
 *      ...
 *      deleteString(&s);
 *      // s is now NULL
 *
 * Parameters:
 * as - the address of a String pointer
 *
 * Return:
 * No return value but a side effect of this function is that the pointer to 
 * the struct string is set to NULL. That is, s in the example above will 
 * be NULL after the call to deleteString.
 *
 * Errors:
 * Not applicable
 */
void deleteString(String* as);

/*
 * Function:
 * printString(const char* format, String s)
 * 
 * Description:
 * A wrapper for the C library printf function (see printf(3) man page).
 * Prints the given String s to stdout according to the given format.
 *
 * Usage: 
 *      String s = newString("hello");
 *      printString("String s: %s\n", s);   // prints "String s: hello\n" to 
 *      ...                                 // stdout
 *      ...
 *      deleteString(&s);
 *
 * Parameters:
 * format - the format of output with %s replaced by string s (see printf(3) 
 *          for details)
 * s - the string to print
 *
 * Return:
 * The number of characters printed not including any terminating '\0'
 *
 * Errors:
 * See printf(3) man page.
 */
int printString(const char* format, String s);

/*
 * Function:
 * fprintString(FILE* stream, const char* format, String s)
 * 
 * Description:
 * A wrapper for the C library fprintf function (see fprintf(3) man page).
 * Prints the given String s to the given open file stream according to the
 * given format.
 *
 * Usage: 
 *      String s = newString("hello");
 *      fprintString(stderr, "String s: %s\n", s); 
 *      ...                         // prints "String s: hello\n" to stderr
 *      ...
 *      deleteString(&s);
 *
 * Parameters:
 * stream - the open file stream to print the string to
 * format - the format of output with %s replaced by string s (see fprintf(3) 
 *          for details)
 * s - the string to print
 *
 * Return:
 * The number of characters printed not including any terminating '\0'
 *
 * Errors:
 * See fprintf(3) man page.
 */
int fprintString(FILE* stream, const char* format, String s);

/*
 * Type definition:
 * struct string - a string whose "member" functions ensure operations on its
 * internal char* are within string length bounds.
 * 
 * Description:
 * The definition of a string type with "member functions":
 *      char_at
 *      concat
 *      equals
 *      get_value
 *      index_of
 *      length
 *      split
 *      substring
 * 
 * The "member functions" are pointers to functions that are 
 * defined in string_o.c.
 * The pointer assignments are made by the newString function.
 * 
 * The member functions operate on values stored in an object map (see
 * obj_map.h) and there is no direct access to the value associated with a
 * struct string.
 *
 * String (defined above) is another name for a pointer to a struct string.
 */
struct string {
    /*
     * Pointer to function member field:
     * char_at(String self, int posn)
     * 
     * Description:
     * Return the character in the given string at the given position or 0 for 
     * the empty string.
     * 
     * Usage: 
     *      String s = newString("hello");
     *      char c = s.char_at(s, 1);       // c will be 'e', assume s not null
     *
     * Parameters:
     * self - the non-null String on which char_at is called (e.g. s in above
     *      example). 
     * posn - the posn in the string to select a char. Valid string positions
     *      are in the range 0 to (length - 1). 0 is a valid position for the 
     *      empty string but (length - 1) = -1 is not.
     *
     * Return:
     * On success: the character at the given position in the string or 0 for 
     *      the empty string
     * On failure: 0 and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, 0 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL or posn is outside  
     *          the valid range for the string.
     */
    char (*char_at)(String self, int posn);

    /*
     * Pointer to function member field:
     * concat(String self, String s)
     * 
     * Description:
     * Concatenate two strings together and return a newly allocated 
     * string that is the result of the concatenation. If the function
     * succeeds, a new string struct is always allocated even if its internal
     * value will be logically equal to one of the arguments to the 
     * concat function. As with newString, it is the responsibility of the user 
     * to subsequently free memory allocated for the returned String 
     * by using deleteString.
     * 
     * Usage: 
     *      String s1 = newString("hello");
     *      String s2 = newString(" again");
     *      String s3 = s1->concat(s1, s2); // s3 value will be "hello again"           
     *      ...                             // assumes s1 and s2 not null
     *      ...
     *      deleteString(&s1);
     *      deleteString(&s2);
     *      deleteString(&s3);
     *
     * Parameters:
     * self - the non-null String on which concat is called (e.g. s1 in above
     *      example). 
     * s - the non-null String to concatenate with self
     *
     * Return:
     * On success: a new non-null String whose value is the concatenation
     *      of the values of self and s 
     * On failure: NULL, and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set to:
     *      EINVAL - invalid argument: if either self or s are NULL
     *      ENOMEM - not enough space: if dynamic allocation of the 
     *          concatenation fails (e.g. for the same reason as newString)
     *      Other errno values to do with object storage of new objects as
     *          indicated in comments to newString.
     */
    String (*concat)(String self, String s);

    /*
     * Pointer to function member field:
     * equals(String self, String s)
     * 
     * Description:
     * Compare two non-null strings for logical equality and return true if 
     * they are equal and false otherwise.
     * Two strings are equal if they have the same length and their internal 
     * arrays of characters are the same.
     * If either or both parameters is NULL, the function will return false.
     * Note: two non-null pointers to the same string object are trivially equal 
     * because they are identical their internal representation will be 
     * identical (i.e. the same objects in memory).
     * 
     * Usage: 
     *      String s1 = newString("hello");     
     *      String s2 = newString("hello");
     *      String s3 = newString("goodbye");
     *      s1.equals(s1, s1);  // true: identical
     *      s1.equals(s1, s2);  // true: not identical but logically equal
     *      s1.equals(s1, s3);  // false: not identical and not logically equal
     *      ...
     *      ...
     *      deleteString(&s1);
     *      deleteString(&s2);
     *      deleteString(&s3);
     *
     * Parameters:
     * self - the non-null String on which equals is called (e.g. s1 in above
     *      examples). 
     * s - the non-null string to compare with self
     *
     * Return:
     * True if self and s are both non-null and logically equal, false
     * otherwise.
     *
     * Errors:
     * Not applicable
     */
    bool (*equals)(String self, String s);
    
    /*
     * Pointer to function member field:
     * get_value(String self, char* buf)
     * 
     * Description:
     * Copy the value of a string to the given buffer (or a newly allocated
     * buffer if buf is NULL) and return a pointer to the buffer. The returned
     * buffer will be NUL-terminated (e.g. will form a valid C string). 
     * If a buf is provided by the user, it is their responsibility to ensure
     * that it is large enough to hold the full length of the string plus the
     * NUL terminator. The function cannot check the size of the user provided
     * buffer.
     * If the buffer buf is NULL then a new copy of the string's char* value
     * is dynamically allocated and it is the user's responsibility to free
     * the allocation when it is no longer required.   
     * 
     * Usage: 
     *      // with a user provided buffer
     *      String s1 = newString("hello");
     *      char buf[s1->length(s1) + 1];     
     *      char* sval1 = s1->get_value(s1, buf); // sval1 and buf are identical
     *      ...
     *      ...
     *      deleteString(&s1);
     *          // buf still exists as an independent copy of the value of s1
     *
     *      // with a NULL pointer instead of a user provided buffer
     *      String s2 = newString("stringtocopy");
     *      char* sval2 = s2->get_value(s2, NULL);  
     *      ...                             // sval2 is dynamically allocated
     *      ...
     *      deleteString(&s2);
     *      free(sval2);        // user is responsible for freeing sval2
     *         
     *
     * Parameters:
     * self - the non-null String on which get_value is called (e.g. s in above
     *      example). 
     * buf - a character buffer large enough to hold the value array of the
     *      string or NULL to request the dynamic allocation of a copy of the 
     *      string
     *
     * Return:
     * On success: a pointer to the buffer containing the copy of the internal
     * char*
     * On failure: NULL, and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, the NULL pointer will be returned and errno will be 
     * set to:
     *      EINVAL - invalid argument: if self is NULL or does not exist
     *      ENOMEM - not enough space: if buf is NULL and dynamic allocation
     *          fails
     */
    char* (*get_value)(String self, char* buf);

    /*
     * Pointer to function member field:
     * index_of(String self, char c, int start)
     * 
     * Description:
     * Return the index position of the first occurrence of the given character
     * in the given string, starting from position start. If the character is
     * not in the string or occurs before the starting position, -1 is 
     * returned.
     * 
     * Usage: 
     *      String s = newString("hello");      // assume s is not null
     *      int l1 = s->index_of(s, 'l', 0);    // l1 will be 2
     *      int l2 = s->index_of(s, 'l', 3):    // l2 will be 3 
     *      ...
     *      ...
     *      deleteString(&s);    
     *
     * Parameters:
     * self - the non-null String on which index_of is called (e.g. s in above
     *      example). 
     * c - the character to find in the string
     * start - the position in the string to start the search from
     *
     * Return:
     * On success: the index of the first occurrence of character c, or 
     *      -1 if the character is not in the string or is before the given 
     *      start position
     * On failure: -2 and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, -2 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL or start is outside  
     *          the valid range for the string.
     */
    int (*index_of)(String self, char c, int start);

    /*
     * Pointer to function member field:
     * length(String self)
     * 
     * Description:
     * Return the length of the string.
     * 
     * Usage: 
     *      String s = newString("hello");      // assume s is not null
     *      int len = s->length(s);             // len will be 5
     *      ...
     *      ...
     *      deleteString(&s);
     *
     * Parameters:
     * self - the non-null String on which length is called (e.g. s in above
     *      example). 
     *
     * Return:
     * On success: the length of the string
     * On failure: -1 and errno will be set to EINVAL
     *
     * Errors:
     * If the call fails, -1 will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL
     */
    int (*length)(String self);
    
    /*
     * Pointer to function member field:
     * split(String self, String delim)
     * 
     * Description:
     * Return a dynamically allocated array of Strings that result from
     * splitting self at any occurrence of any character in the delim String. 
     * The array of splits will have one entry for each string of characters 
     * either side of any one of the delimiter characters in delim. This means
     * that a string of consecutive delimiters in the input string, self, will 
     * result in multiple empty strings in the returned array. See usage below
     * for an example.
     * If the input string, self, does not contain any of the characters in the 
     * delim string then an array with a single entry that is a copy of self 
     * is returned.
     * The function guarantees that there is at least one NULL sentinel value 
     * in the returned array that follows all the valid splits. Therefore, 
     * the array will have one or more valid entries that result from the 
     * split operation followed by at least one NULL entry to determine the 
     * end of the list of valid entries. See usage below for an example.
     * It is the user's responsibility to free the memory allocated for each
     * member of the returned array (using deleteString) and to free the 
     * memory allocated for the array itself (see usage below).
     * 
     * Usage: 
     *      String s = newString("/w/x:y////z"); // assume s is not null
     *      String* tokens = s->split(s, "/:");  // splits s by / or :
     *          // tokens is an array that contains Strings that encapsulate 
     *          // the following char* values, followed by at least one NULL
     *          // sentinel: 
     *          // {"", "w", "x", "y","", "", "", "z", NULL, ...}
     *      
     *      for (int i = 0; tokens[i] != NULL; i++)
     *          printString("%s\n", tokens[i]);      
     *              // prints "", "w", "x", "y","", "", "", "z" 
     *              // on separate lines
     *
     *      for (int j = 0; j < i; j++)
     *          deleteString(&tokens[j]);
     *
     *      free(tokens); 
     *
     * Note: the above usage is illustrated in the string_app.c program.
     *
     * Parameters:
     * self - the non-null String to split (e.g. s in above example). self can
     *      be the empty string but cannot be NULL.
     * delim - the non-null string of delimiter characters. delim can
     *      be the empty string but cannot be NULL.
     *
     * Return:
     * On success: an array of strings of one or more splits of the input string
     *      followed by a NULL sentinel. 
     * On failure: NULL and errno will be set to one of the values specified 
     *      under Errors. Note: it is possible that the allocation of a 
     *      new String will fail due to memory errors. In this case, the 
     *      corresponding entry in the returned array will be NULL and there    
     *      will be no more valid entries after the allocation failure. That is, 
     *      in the case of failure to allocate a String member of the array, 
     *      the array may be incomplete but NULL entries can still be detected.
     *
     * Errors:
     * If the call fails, NULL will be returned and errno will be set to:
     *      EINVAL - invalid argument: if either self or delim is NULL
     *      ENOMEM - not enough space: if dynamic allocation of the array or 
     *          its members fails (in which case the note above re. the 
     *          return of an incomplete array in event of a falure applies).
     */
    String* (*split)(String self, String delim);

    /*
     * Pointer to function member field:
     * substring(String self, int start, int length)
     * 
     * Description:
     * Return a newly allocated substring of self from the given 
     * start index for length characters (not including the NUL terminator
     * character). 
     * The valid range for the start index for the substring is: 
     *      0 <= start <= self->length(self) - 1
     * The valid range for the length of the substring is:
     *      0 <= length <= self->length(self) - start
     * A start index of 0 and length of self->length(self) will return a copy 
     * of the input string. A length of 0 and any valid start index will result
     * in return of the empty string.
     * 
     * Usage: 
     *      String s = newString("hello");          // assume s is not null
     *      String ss1 = s->substring(s, 0, 5); 
     *      printString("ss1: %s\n", ss1);          // prints "ss1: hello"
     *      String ss2 = s->substring(s, 0, 2): 
     *      printString("ss2: %s\n", ss2);          // prints "ss2: he"
     *      String ss3 = s->substring(s, 3, 2);     
     *      printString("ss3: %s\n", ss2);          // prints "ss3: lo"
     *      ...
     *      ...
     *      deleteString(s);
     *      deleteString(ss1);
     *      deleteString(ss2);
     *      deleteString(ss3);
     *
     * Parameters:
     * self - the non-null String from which to create a substring (e.g. s 
     *      in the example above)
     * start - the start index in self from where to start the substring. 
     *      start must be in the range 0 to self->length(self) - 1
     * length - the length of the substring, which must be in the range
     *      0 to self->length(self) - start
     * 
     * Return:
     * On success: the newly specified substring
     * On failure: NULL and errno will be set to one of the values specified 
     * under Errors.
     *
     * Errors:
     * If the call fails, NULL will be returned and errno will be set to:
     *      EINVAL - invalid argument: if self is NULL or either start or
     *          length is outside their valid range
     *      ENOMEM - not enough space: if the substring cannot be allocated
     */
    String (*substring)(String self, int start, int length);
};

/*  
 * Function:
 * _test_string_val(String s)
 * 
 * Description:
 * Provides access to a string's underlying char* value to simplify tests of
 * String. This function will be removed in production versions of String and 
 * is NOT part of the public interface of a String. Do NOT use this function
 * except in tests of String.
 * 
 * Usage:
 *      String s = newString("hello");
 *      char* sval = _test_string_val(s);   // sval == "hello"
 * 
 * Parameters:
 * s - the string from which to obtain the underlying value
 *
 * Return: 
 * The underlying char* value of the given string object (as opposed to the
 * copy of the value returned by get_value) or NULL if s does not exist.
 */
const char* _test_string_val(String s);
 
#endif