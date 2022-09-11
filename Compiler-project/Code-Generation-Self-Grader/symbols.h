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
void add_command(char *cmd, int n, int sp);
int compile_file(char *name);
int get_method_args(Token class_name, Token method_name);
int get_method_locals(Token class_name, Token method_name);
int get_class_fields(Token class_name);
char* joinstr(char* a, char* b);
Token jointoken(char* cmd, Token t1, Token t2);
int get_field_address(Token class_name, Token symb);
int get_agument_address(Token class_name, Token method_name, Token symb);
int get_local_address(Token class_name, Token method_name, Token symb);
Token get_class_name(Token class_name, Token method_name, Token tpe);
int get_type_index(Token tk);
int get_static_address(Token class_name, Token symb);
int check_method_void(Token class_name, Token method_name);

extern int COMPILE_FLAG; 
#endif
