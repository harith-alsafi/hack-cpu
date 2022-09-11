#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below
int init_symbols();
void set_class_name(Token name);
void add_class();
void add_method_to_class(Token method_type, Token return_type, Token name);
void add_arg_to_method(Token type, Token tk);
void add_var_to_method(Token type, Token tk);
void add_var_to_class(Token kind, Token type, Token tk);
int get_class_index(Token tk);
int check_current_method_scope(Token tk);
int check_current_class_scope(Token tk);
int check_current_class_methods(Token tk);
void delete_symbols();
ParserInfo check_symbols(char* dir_name);
extern int COMPILE_FLAG; 
#endif
