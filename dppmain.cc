/* File: dppmain.cc
 * ----------------
 * This file defines the main() routine for the preprocessor, 
 * the filtering tool which runs before the compiler.
 */

#include "scanner.h"
#include <stdio.h>
#include "errors.h"
#include <iostream>
#include <map>
#include <string>


using namespace std;

namespace global {
    // variables globales porque me hago bolas con los apuntadores :D
    int lines = 0;
    char def[8] = "define ";
    map<string, string> directives;

}

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
    // ignorar los caracteres hasta llegar al newline
    while ((ch = getc(stdin)) != '\n');
    putc(ch, stdout);
}

int multiple() {
    int prev, curr;
    while ((curr = getc(stdin)) != EOF) {
        // si se llega al newline, imprimirlo
        if (curr == '\n')
            putc(curr, stdout);
        // si se llega a una diagonal se checa que el anterior haya sido un asterisco
        // si lo es entonces se cierra el comentario y se regresa a la funcion principal
        if (curr == '/')
            if (prev == '*')
                break;

        prev = curr;
    }
    
    // se marca error si no se cierra un comentario multiple
    if (curr == EOF)
        return -1;
    else
        return 0;
    
}



int add_directive() {
    int ch;
    int j = 0;
    char name[32];
    char value[81];

    // se agrega el nombre de la directiva
    while (j < 31 && ((ch = getc(stdin)) >= 'A' && ch <= 'Z')) {
        name[j] = ch;
        j++;
    }
    name[j] = '\0'; // se agrega el null para poder convertirlo a string

    // se verifica que este bien declarada la directiva nueva
    if (j < 31) {
        if (ch == ' ') { // si lo esta entonces agrega su valor al diccionario
            j = 0;
            while (j < 80 && ((ch = getc(stdin)) != '\n')) {
                value[j] = ch;
                j++;
            }
            value[j] = '\0'; // se agrega el null para poder convertirlo a string

            if (j < 80) {
                global::directives[name] = value;
            } else {
                // error de longitud
            }
            putc('\n', stdout);
        }
    } else {
        // error de longitud
    }
}

int use_directive() {
    int  j = 0;
    int ch;
    char directive[32];

    while (j < 31 && ((ch = getc(stdin)) >= 'A' && ch <= 'Z')) {
        directive[j] = ch;
        j++;
    }
    directive[j] = '\0';
    string directive_s(directive);

    
    if (j < 31) {
        map<string, string>::iterator ii=global::directives.begin();
        for (ii; ii!=global::directives.end(); ++ii) {
            if ((*ii).first.compare(directive_s) == 0) {
                cout << (*ii).second;
                break;
            }
        }

        if (ii==global::directives.end()) {
            // error de directiva no encontrada
        }
        putc(ch, stdout);
    } else {
        // error de longitud
    }

    return 0;
}


int main(int argc, char *argv[]) {

    int ch, prev;
    int ch2 = 0;
    int i = 0;
    int return_code = 0;

    // verificacion que se conserve la diagonal como operador (este mismo archivo es el de prueba)
    int prueba = (2/2);


    while ((ch = getc(stdin)) != EOF) {
        // diagonal encontrada
        if (ch == '/') {
            ch2 = getc(stdin);
            // comentario de una linea
            if (ch2 == '/') {
                sencillo();
                prev = '\n';
            } else if (ch2 == '*') { // comentario de varias lineas
                return_code = multiple();
                prev = '\n';
            } else { // no comentario (diagonal como operador, etc)
                putc(ch, stdout);
                putc(ch2, stdout);
                prev = ch2;
            }
        } else if (ch == '#') { // gato encontrado
            if (prev == '\n') {
                int j = 0;
                while (j < 7 && (ch = getc(stdin)) == global::def[j])
                    j++;
                
                if (j == 7)
                    add_directive();
                else {
                    putc('#', stdout);
                    putc(ch, stdout); //raise_error(); // levantar alerta y deshacerse de la linea
                }
                prev = '\n';
            } else if (prev == ' ') { // revisar que caracteres que pueden ir antes del uso de una directiva
                prev = use_directive();
            }
        }else { // otros caracteres pasan sin modificar
            putc(ch, stdout);
            prev = ch;
        }

        i++;
    }
    return return_code;
}
