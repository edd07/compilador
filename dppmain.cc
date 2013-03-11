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
#include <vector>


using namespace std;

namespace global {
    // variables globales porque me hago bolas con los apuntadores :D
    int lines = 1;
    int unclosed_string = 1;
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
    if (ch == '\n')
        global::lines++;
    putc(ch, stdout);
    
}

void multiple() {
    int prev, curr;
    while ((curr = getc(stdin)) != EOF) {
        // si se llega al newline, imprimirlo
        if (curr == '\n') {
            putc(curr, stdout);
            global::lines++;
        }
        // si se llega a una diagonal se checa que el anterior haya sido un asterisco
        // si lo es entonces se cierra el comentario y se regresa a la funcion principal
        if (curr == '/')
            if (prev == '*')
                break;

        prev = curr;
    }
    
    // se marca error si no se cierra un comentario multiple
    if (curr == EOF) {
        //ReportError::UntermComment();
    }   
}



void add_directive() {
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
            // se lee el valor de la directiva
            while (j < 80 && ((ch = getc(stdin)) != '\n')) {
                value[j] = ch;
                j++;
            }
            value[j] = '\0'; // se agrega el null para poder convertirlo a string

            // se verifica la longitud del valor de la directiva
            if (j < 80) {
                global::directives[name] = value;
            } else {
                ////ReportError::InvalidDirective(global::lines);
            }
            putc('\n', stdout);
            global::lines++;
        }
    } else { // longitud invalida
        //ReportError::InvalidDirective(global::lines);
    }
}

int use_directive() {
    int  j = 0;
    int ch;
    char directive[32];

    // se lee el nombre de la directiva
    while (j < 31 && ((ch = getc(stdin)) >= 'A' && ch <= 'Z')) {
        directive[j] = ch;
        j++;
    }
    directive[j] = '\0';
    string directive_s(directive); // se convierte a un string

    // se verifica la longitud del nombre
    if (j < 31) {

        map<string, string>::iterator ii=global::directives.begin();

        // se busca en el diccionario la directiva
        if (global::directives.find(directive) == global::directives.end()) {
            //ReportError::InvalidDirective(global::lines);
        } else
            cout << global::directives[directive];
        

        if (ch == '\n') 
            global::lines++;

        putc(ch, stdout);
        
    } else { // longitud invalida
        //ReportError::InvalidDirective(global::lines);
    }

    return ch;
}

int choose_directive() {
    vector<char> tmp(8);
    int j = 0;
    int ch = 0;
    int prev;
    int invalid_directive = 0;

    while (j < 7 && (ch = getc(stdin)) == global::def[j]) {
        tmp[j] = ch;
        j++;
    }
    tmp[j] = ch;

    // putc(6, stdout);
    // for (int tmp_i = 0; tmp_i < j+1; tmp_i++) {
    //     putc(tmp[tmp_i], stdout);
    // }
    // putc(6, stdout);
    
    if (j == 7) {
        add_directive();
        prev = '\n';
    } else {
        for (int tmp_i = 0; tmp_i < +1; tmp_i++) { // checa si todo lo que leyo es mayuscula
            if (!(tmp[tmp_i] >= 'A' && tmp[tmp_i] <= 'Z')) {
                invalid_directive = 1; // si no, es una directiva invalida
            }
        }

        // si no es #NAME entonces imprime lo que leyo
        if (invalid_directive) {
            // putc('X', stdout);
            putc('#', stdout);
            for (int tmp_i = 0; tmp_i < j+1; tmp_i++) { 
                if (tmp[tmp_i] == '\n') 
                    global::lines++;

                putc(tmp[tmp_i], stdout);
                prev = tmp[tmp_i];
            }
        } else {
            // putc('X', stdout);
            // putc('X', stdout);
            for (int tmp_i = j; tmp_i >= 0; tmp_i--) {

                ungetc(tmp[tmp_i], stdin);
            }
            prev = use_directive();
        }
    }

    invalid_directive = 0;
    return prev;
}


int main(int argc, char *argv[]) {

    int ch, prev;
    int ch2 = 0;
    int i = 0;


    while ((ch = getc(stdin)) != EOF) {
        // diagonal encontrada
        if (ch == '/') {
            ch2 = getc(stdin);
            // comentario de una linea
            if (ch2 == '/') {
                sencillo();
                prev = '\n';
            } else if (ch2 == '*') { // comentario de varias lineas
                multiple();
                prev = '\n';
            } else { // no comentario (diagonal como operador, etc)
                if (ch == '\n')
                    global::lines++;

                putc(ch, stdout);
                

                if (ch2 == '\n')
                    global::lines++;

                putc(ch2, stdout);
                
                prev = ch2;
            }
        } else if (ch == '#') { // directiva encontrada (posiblemente)
            // definicion de directiva
            if (prev == '\n' || global::lines == 1)
                prev = choose_directive();
            else // uso de directiva porque no esta al inicio de la linea
                prev = use_directive();

        } else { // otros caracteres pasan sin modificar
            if (ch == '\n')
                global::lines++;

            putc(ch, stdout);
            prev = ch;
        }

        i++;
    }
    return 0;
}
