/* File: dppmain.cc
 * ----------------
 * This file defines the main() routine for the preprocessor, 
 * the filtering tool which runs before the compiler.
 */

#include "scanner.h"
#include <stdio.h>

/* Function: main()
 * ----------------
 * Entry point to the preprocessor.
 * As given below, this "filter" just copies stdin to stdout unchanged.
 * It will be your job to actually do some filtering, either by hand-coded
 * transformation of the input or by setting up a lex scanner in the dpp.l
 * file and changing the main below to invoke it via yylex. When finished,
 * the preprocessor should echo stdin to stdout making the transformations
 * to strip comments and handle preprocessor directives.
 */

void sencillo() {
    int ch;
    while ((ch = getc(stdin)) != 10);
    putc(ch, stdout);
}

int multiple() {
    int prev, curr;
    while ((curr = getc(stdin)) != EOF) {
        if (curr == 10)
            putc(curr, stdout);

        if (curr == 47)
            if (prev == 42)
                break;

        prev = curr;
    }
    
    if (curr == EOF)
        return -1;
    else
        return 0;
    
}

int main(int argc, char *argv[]) {

    int ch, ch2;
    int i = 0;
    int return_code = 0;

    int prueba = 2/2;


    while ((ch = getc(stdin)) != EOF) {
        if (ch == 47) {
            ch2 = getc(stdin);
            if (ch2 == 47) {
                sencillo();
            } else if (ch2 == 42) {
                return_code = multiple();
            } else {
                putc(ch, stdout);
                putc(ch2, stdout);
            }
        } else {
            putc(ch, stdout);
        }
        i++;
    }
    return return_code;
}
