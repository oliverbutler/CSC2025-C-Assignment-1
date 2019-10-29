#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "../string_o.h"

/* 
 * Program to demonstrate example usage of String (as defined in string_o.h).
 */

int main(void) {
    String s = newString("hello");  /* strictly, should check this succeeds */
    String t = newString("again");
    String sp = newString(" ");
    printf("s length: %d\n", s->length(s));
    printf("t length: %d\n", t->length(t));
    
    String u = s->concat(s, sp);
    printf("u length: %d\n", u->length(u));
    
    String v = u->concat(u, t);
    printf("v length: %d\n", v->length(v));
    
    char sv[s->length(s)+1];
    char tv[t->length(t)+1];
    char vv[v->length(v)+1];
    
    s->get_value(s, sv);
    t->get_value(t, tv);
    v->get_value(v, vv);
    
    printf("sv (%d): \"%s\"\n", (int) strnlen(sv, STR_LEN_MAX), sv);
    printf("tv (%d): \"%s\"\n", (int) strnlen(tv, STR_LEN_MAX), tv);
    printf("vv (%d): \"%s\"\n", (int) strnlen(vv, STR_LEN_MAX), vv);
    
    for (int i = 0; i < s->length(s); i++) 
        printf("%c\n", s->char_at(s, i));
            
    char c = s->char_at(s, s->length(s));
    
    if (!c)
        perror("char_at error");
    
    int idx = 0;
    c = 'l';
    int start = idx;
    int count =0;
    while ((idx = s->index_of(s, c, start)) != -1) {
        count++;
        printf("index of number %d %c in %s is %d\n", count, c,
            sv, idx);
        start = idx + 1;
    }
    
    String s2 = newString("hello");
    
    printf("s.equals(s2) is %s\n", s->equals(s, s2) ? "true" : "false");
        
    deleteString(&s);
    printf("deleted s\n");
    deleteString(&s2);
    printf("deleted s2\n");
    deleteString(&t);
    printf("deleted t\n");
    deleteString(&sp);
    printf("deleted sp\n");
    deleteString(&u);
    printf("deleted u\n");
    deleteString(&v);    
    printf("deleted v\n");
    
    s = newString(""); // assume s is not null
    String delim = newString("/:");
    String* tokens = s->split(s, delim);  // splits s by 
    printString("s: %s\n", s);

    int i;
    for (i = 0; tokens[i] != NULL; i++) {
        printf("token[%d]: ", i);
        printString("%s\n", tokens[i]);  
    }
    
    for (int j = 0; j < i; j++)
        deleteString(&tokens[j]);
    
    free(tokens);

    deleteString(&s);
    deleteString(&delim);
      
    s = newString("hello"); // assume s is not null
    
    String hello = s->substring(s, 0, 5);
    String ello = s->substring(s, 1, 4);
    String hell = s->substring(s, 0, 4);
    String h = s->substring(s, 0, 1); 
    String he = s->substring(s, 0, 2);
    String o = s->substring(s, 4, 1);
    String el = s->substring(s, 1, 2);
    String empty = s->substring(s, 1, 0);
                            
    printString("hello: %s\n", hello);  
    printString("ello: %s\n", ello);  
    printString("hell: %s\n", hell);  
    printString("h: %s\n", h);  
    printString("he: %s\n", he);  
    printString("o: %s\n", o);  
    printString("el: %s\n", el);  
    printString("empty: %s\n", empty);  
    printString("s: %s\n", s);  
    
    deleteString(&s);
    deleteString(&hello);
    deleteString(&ello);
    deleteString(&hell);
    deleteString(&h);
    deleteString(&he);
    deleteString(&o);
    deleteString(&el);
    deleteString(&empty);

    return 0;
} 