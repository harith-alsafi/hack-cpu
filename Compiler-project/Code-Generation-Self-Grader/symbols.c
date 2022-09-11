
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Harith alsafi
Student ID: 201416467 
Email: el20hzaa@leeds.ac.uk
Date Work Commenced: 01/04/2022
*************************************************************************/

#include "symbols.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

/**
 * @brief Holds id name and its type
 * 
 */
typedef struct{
    Token type; // int | char .. etc
    Token id;
    Token kind; // static or field 
}Identifier;

/**
 * @brief Dynamic array of variables
 * 
 */
typedef struct{
    size_t size;
    Identifier *data;
}Variable;

/**
 * @brief All information for method
 * 
 */
typedef struct {
    Token method_name; // method name 
    Variable arguments; // arguments 
    Variable var; // local variables
    Token method_return; // method return 
    Token method_type; // method | function | constructor
}Subroutine;

/**
 * @brief Dynamic array of methods
 * 
 */
typedef struct {
    size_t size;
    Subroutine* data;
}Method;

/**
 * @brief Information for class
 * 
 */
typedef struct{
    Token class_name; // class name 
    Method methods; // class methods 
    Variable vars; // class variables 
    Variable static_vars; // static var
    Variable field_vars; // field vars
}Class;

/**
 * @brief Dynamic array of classes 
 * 
 */
typedef struct {
    size_t size;
    Class *data;
}Symbol;

typedef struct{
    size_t size;
    char **data;
}Commands;


// The table
Symbol symbols;

// Commands list 
Commands commands;


/**
 * @brief pushes a new class into the table 
 * 
 * @param stack class array
 * @param val class value
 */
void push_class(Symbol *stack, Class val) {
    stack->size++;
    Class *new_data = realloc(stack->data, stack->size * sizeof *stack->data);

    if (!new_data) {
        // error handling here, or just
        abort();
    }
    
    stack->data = new_data;
    stack->data[stack->size - 1] = val;
}

/**
 * @brief Pushes a variable into the dynamic variable array
 * 
 * @param stack variable array
 * @param val variable value
 */
void push_var(Variable *stack, Identifier val){
    stack->size++;
    Identifier *new_data = realloc(stack->data, stack->size * sizeof *stack->data);

    if (!new_data) {
        // error handling here, or just
        abort();
    }
    
    stack->data = new_data;
    stack->data[stack->size - 1] = val;
}

/**
 * @brief Pushes new method to array of methods
 * 
 * @param stack Methods array
 * @param val method value
 */
void push_method(Method* stack, Subroutine val){
    stack->size++;
    Subroutine *new_data = realloc(stack->data, stack->size * sizeof *stack->data);

    if (!new_data) {
        // error handling here, or just
        abort();
    }
    
    stack->data = new_data;
    stack->data[stack->size - 1] = val;
}


/**
 * @brief Initialises symbol data and adds built in libraries
 * 
 */
int init_symbols(){
    symbols.size = 0;
    symbols.data = NULL;
    commands.size = 0;
    commands.data = NULL;
	COMPILE_FLAG = 0;
    ParserInfo p;

    add_class();
    InitParser("Array.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Keyboard.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Output.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("String.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Sys.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Screen.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Math.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    add_class();
    InitParser("Memory.jack");
    p = Parse();
    StopParser();
    if(p.er != none){
        return 0;
    }

    return 1;
}

/**
 * @brief Adds new class 
 * 
 */
void add_class(){
    Class class;
    class.methods.size = 0;
    class.methods.data = NULL;
    class.vars.size = 0;
    class.vars.data = NULL;
    class.static_vars.data = NULL;
    class.static_vars.size = 0;
    class.field_vars.data = NULL;
    class.field_vars.size = 0;
    push_class(&symbols, class);
}

/**
 * @brief Set the class name object
 * 
 * @param name 
 */
void set_class_name(Token name){
    symbols.data[symbols.size-1].class_name = name;
}

/**
 * @brief Add new method to current class 
 * 
 * @param method_type // function | constructor | method
 * @param return_type // void | type
 * @param name // ID
 */
void add_method_to_class(Token method_type, Token return_type, Token name){
    Subroutine sub;
    sub.arguments.data = NULL;
    sub.arguments.size = 0;
    sub.var.data = NULL;
    sub.var.size = 0;
    sub.method_name = name;
    sub.method_return = return_type;
    sub.method_type = method_type;
    push_method(&symbols.data[symbols.size-1].methods, sub);
}

/**
 * @brief Adds argument to method
 * 
 * @param type // type
 * @param tk // ID
 */
void add_arg_to_method(Token type, Token tk){
    Identifier id;
    id.id = tk;
    id.type = type;
    push_var(&symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].arguments, id);
}

/**
 * @brief Adds local variable to method
 * 
 * @param type // Type
 * @param tk // ID
 */
void add_var_to_method(Token type, Token tk){
    Identifier id;
    id.id = tk;
    id.type = type;
    push_var(&symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].var, id);
}

/**
 * @brief Adds new class variable
 * 
 * @param kind // static | field 
 * @param type // int | char .. etc 
 * @param tk // ID
 */
void add_var_to_class(Token kind, Token type, Token tk){
    Identifier id;
    id.id = tk;
    id.type = type;
    id.kind = kind;
    if(strcmp(kind.lx, "static") == 0){
        push_var(&symbols.data[symbols.size-1].static_vars, id);
    }
    else{
        push_var(&symbols.data[symbols.size-1].field_vars, id);
    }
    push_var(&symbols.data[symbols.size-1].vars, id);
}

/**
 * @brief Get the class index object
 * 
 * @param tk 
 * @return int 
 */
int get_class_index(Token tk){
    for(int i = 0; i < symbols.size; i++){
        if(strcmp(symbols.data[i].class_name.lx, tk.lx) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Checks the attributes of a given class index 
 * Attributes -> (methods + variables)
 * @param indx 
 * @param tk 
 * @return int 
 */
int check_given_class_attributes(int indx, Token tk){
    // methods 
    for(int i = 0; i < symbols.data[indx].methods.size; i++){
        if(strcmp(symbols.data[indx].methods.data[i].method_name.lx, tk.lx) == 0){
            return 1;
        }
    } 

    // variables
    for(int i = 0; i < symbols.data[indx].vars.size; i++){
        if(strcmp(symbols.data[indx].vars.data[i].id.lx, tk.lx) == 0){
            return 1;
        }
    } 
    return 0;
}

/**
 * @brief Checks if method name exists in current class
 * 
 * @param tk 
 * @return int 
 */
int check_current_class_methods(Token tk){
    for(int i = 0; i < symbols.data[symbols.size-1].methods.size; i++){

        // method name 
        if(strcmp(symbols.data[symbols.size-1].methods.data[i].method_name.lx, tk.lx) == 0){
            return 1;
        }
    }
    return 0;
}

/**
 * @brief checks all variables and methods of current class
 * 
 * @param tk 
 * @return int 
 */
int check_current_class_scope(Token tk){
    // checking all class variables 
    for(int i = 0; i < symbols.data[symbols.size-1].vars.size; i++){
        if(strcmp(symbols.data[symbols.size-1].vars.data[i].id.lx, tk.lx) == 0){
            return 1;
        }
    }
    // checking all methods 
    return check_current_class_methods(tk);
}

/**
 * @brief Checks arguments and local variables of current method
 * 
 * @param tk 
 * @return int 
 */
int check_current_method_scope(Token tk){
    // checking current method
    // args
    for(int j = 0; j < symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].arguments.size; j++){
        if(strcmp(symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].arguments.data[j].id.lx, tk.lx) == 0){
            return 1;
        }
    }
    // local variables 
    for(int j = 0; j < symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].var.size; j++){
        if(strcmp(symbols.data[symbols.size-1].methods.data[symbols.data[symbols.size-1].methods.size-1].var.data[j].id.lx, tk.lx) == 0){
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Deletes the table 
 * 
 */
void delete_symbols(){
    free(symbols.data);
    symbols.data = NULL;
    symbols.size = 0;
}

/**
 * @brief Get the type index (if class)
 * 
 * @param tk 
 * @return int 
 */
int get_type_index(Token tk){
    // main types
	if(strcmp(tk.lx, "int") != 0 && 
	strcmp(tk.lx, "char") != 0 &&
	strcmp(tk.lx, "boolean") != 0 &&
    strcmp(tk.lx, "void") != 0){
		for(int i = 0; i < symbols.size; i++){
            if(strcmp(tk.lx, symbols.data[i].class_name.lx) == 0){
                return i;
            }
        }
        return -1;
	}

    return -2;
}

/**
 * @brief Get the method index for given class index and token
 * 
 * @param cindx 
 * @param tk 
 * @return int 
 */
int get_method_index(int cindx, Token tk){
    for(int i = 0; i < symbols.data[cindx].methods.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[i].method_name.lx, tk.lx) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Checks variables of given method index 
 * upon given class index 
 * 
 * @param cindx 
 * @param mindx 
 * @param tk 
 * @return int 
 */
int check_given_method_vars(int cindx, int mindx, Token tk){
    // agrs
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].arguments.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].arguments.data[i].id.lx, tk.lx) == 0){
            return 1;
        }
    }

    // vars
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].var.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].var.data[i].id.lx, tk.lx) == 0){
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Get the var type index object
 * 
 * @param cindx 
 * @param mindx 
 * @param tk 
 * @return int 
 */
int get_var_type_index(int cindx, int mindx, Token tk){
    // classs vars
    for(int i = 0; i < symbols.data[cindx].vars.size; i++){
        if(strcmp(symbols.data[cindx].vars.data[i].id.lx, tk.lx) == 0){
            return get_type_index(symbols.data[cindx].vars.data[i].type);
        }
    }
    // agrs
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].arguments.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].arguments.data[i].id.lx, tk.lx) == 0){
            return get_type_index(symbols.data[cindx].methods.data[mindx].arguments.data[i].type);
        }
    }

    // vars
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].var.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].var.data[i].id.lx, tk.lx) == 0){
            return get_type_index(symbols.data[cindx].methods.data[mindx].var.data[i].type);
        }
    }

    return -1;
}

/**
 * @brief Used to check if a static class call was made like Array.new
 * 
 * @param indx 
 * @param tk 
 * @return int 
 */
int check_class_static_call(int indx, Token tk){
    // methods 
    for(int i = 0; i < symbols.data[indx].methods.size; i++){
        if(strcmp(symbols.data[indx].methods.data[i].method_name.lx, tk.lx) == 0){
            if(strcmp(symbols.data[indx].methods.data[i].method_type.lx, "method") == 0){
                return 0;
            }
            return 1;
        }
    } 

    // variables
    for(int i = 0; i < symbols.data[indx].vars.size; i++){
        if(strcmp(symbols.data[indx].vars.data[i].id.lx, tk.lx) == 0){
            if(strcmp(symbols.data[indx].vars.data[i].kind.lx, "field") == 0){
                return 0;
            }
            return 1;
        }
    } 

    return 0;
}


/**
 * @brief Checks the symbol table 
 * 
 * @return ParserInfo 
 */
ParserInfo check_file(){
    ParserInfo p;
    p.er = none;
    Token next_token;

    next_token = PeekNextToken();
    int class_indx = -1;
    int method_indx = -1;
    while (next_token.tp != EOFile)
    {
        next_token = GetNextToken();

        // Getting class index 
        if(strcmp("class", next_token.lx) == 0){
            class_indx = get_class_index(GetNextToken()); 
        }

        // getting method index 
        if(strcmp(next_token.lx, "constructor") == 0 || 
        strcmp(next_token.lx, "function") == 0 || 
        strcmp(next_token.lx, "method") == 0){
            GetNextToken(); // get rids of void/type
            method_indx = get_method_index(class_indx, GetNextToken());
        }

        if(class_indx != -1 && method_indx != -1){
            // if we come across any ID
            if(next_token.tp == ID){
                // if we have (.) after it 
                if(strcmp(PeekNextToken().lx, ".") == 0){
                    GetNextToken();
                    // gets the class index (incase the ID was a class)
                    int i = get_class_index(next_token); 
                    // gets the type incase the ID was a variable 
                    int j = get_var_type_index(class_indx, method_indx, next_token);
                    
                    // the ID was a class name
                    if(i != -1){
                        next_token = GetNextToken();
                        // checks the attributes of the class ID + check if the token is static or function 
                        if(check_class_static_call(i, next_token) == 0){
                            p.tk = next_token;
                            p.er = undecIdentifier;
                            return p;
                        }
                        continue;
                    }

                    // if the ID was a variable 
                    if(j != -1){
                        next_token = GetNextToken();
                        // check the type of that variable 
                        if(check_given_class_attributes(j, next_token) == 0){
                            p.tk = next_token;
                            p.er = undecIdentifier;
                            return p;
                        }
                        continue;
                    }
                }
                // Doesnt match any type + doesnt match current class and method scope
                if(get_type_index(next_token) == -1 && 
                check_given_class_attributes(class_indx, next_token) == 0 &&
                check_given_method_vars(class_indx, method_indx, next_token) == 0){                    
                    p.tk = next_token;
                    p.er = undecIdentifier;
                    return p;
                }
            }

        }
    }

    return p;
}


/**
 * @brief USed to call check_file for all files in directory 
 * 
 * @param dir_name 
 * @return ParserInfo 
 */
ParserInfo check_symbols(char* dir_name)
{

	ParserInfo p;
    p.er = none;

    // checking all variable types 
    for(int i = 0; i < symbols.size; i++){
        // class vars 
        for(int j = 0; j < symbols.data[i].vars.size; j++){
            if(get_type_index(symbols.data[i].vars.data[j].type) == -1){
                p.er = undecIdentifier;
                p.tk = symbols.data[i].vars.data[j].type;
                return p;
            }
        }

        // class methods
        for(int j = 0; j < symbols.data[i].methods.size; j++){
            // returns 
            if(get_type_index(symbols.data[i].methods.data[j].method_return) == -1){
                p.er = undecIdentifier;
                p.tk = symbols.data[i].methods.data[j].method_return;
                return p;
            }

            // arguments 
            for(int k = 0; k < symbols.data[i].methods.data[j].arguments.size; k++){
                if(get_type_index(symbols.data[i].methods.data[j].arguments.data[k].type) == -1){
                    p.er = undecIdentifier;
                    p.tk = symbols.data[i].methods.data[j].arguments.data[k].type;
                    return p;
                }
            }

            // variables 
            for(int k = 0; k < symbols.data[i].methods.data[j].var.size; k++){
                if(get_type_index(symbols.data[i].methods.data[j].var.data[k].type) == -1){
                    p.er = undecIdentifier;
                    p.tk = symbols.data[i].methods.data[j].var.data[k].type;
                    return p;
                }
            }
        }

    }

	// write your code below
	DIR* folder = opendir(dir_name);
	struct dirent *entry;
	if(folder == NULL){
		return p;
	}

	char name[128];
	strcpy(name, dir_name);

	// reading all files 
	while (entry = readdir(folder))
	{	
		strcpy(name, dir_name);
		// checking for .jack files
		if(strstr(entry->d_name, ".jack")){
			// Parsing 
			strcat(name, "/");
			strcat(name, entry->d_name);

			InitLexer(name);
			p = check_file();
			StopLexer();

			if(p.er != none){
				return p;
			} 
		}
	}
	closedir(folder);
    return p;
}

int get_method_args(Token class_name, Token method_name){
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    return (int) symbols.data[cindx].methods.data[mindx].arguments.size;
}

/**
 * @brief Get number of local vars in method
 * 
 * @param class_name 
 * @param method_name 
 * @return int 
 */
int get_method_locals(Token class_name, Token method_name){
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    return (int) symbols.data[cindx].methods.data[mindx].var.size;
}

/**
 * @brief Get number of class field variables
 * 
 * @param class_name 
 * @return int 
 */
int get_class_fields(Token class_name){
    int cindx = get_class_index(class_name);
    return (int) symbols.data[cindx].field_vars.size;
}

/**
 * @brief Pushes new value to the commands
 * 
 * @param stack 
 * @param val 
 */
void push_command(Commands *stack, char* val) {
    stack->size++;
    char** new_data = realloc(stack->data, stack->size * sizeof *stack->data);

    if (!new_data) {
        // error handling here, or just
        abort();
    }
    
    stack->data = new_data;
    stack->data[stack->size - 1] = val;
}

/**
 * @brief Adds new command 
 * 
 * @param cmd 
 * @param n 
 * @param sp 
 */
void add_command(char *cmd, int n, int sp){
    char *num = (char*) malloc(5);
    char *c = (char*) malloc(strlen(num) + strlen(cmd) + 5);
    strcpy(c, cmd);

    if(n == -1){
        strcat(c, "\n");
    }
    else{
        if(sp){
            sprintf(num, " %i\n", n);
        }
        else{
            sprintf(num, "%i\n", n);
        }
        strcat(c, num);
    }
    push_command(&commands, c);
}

/**
 * @brief Get the class name object
 * 
 * @param class_name 
 * @param method_name 
 * @param tpe 
 * @return Token 
 */
Token get_class_name(Token class_name, Token method_name, Token tpe){
    int indx = get_type_index(tpe);
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    if(indx != -1){
        return symbols.data[indx].class_name;
    }
    else{
        indx = get_var_type_index(cindx, mindx, tpe);
        if(indx != -1){
            return symbols.data[indx].class_name;
        }
    }
    Token tp;
    return tp;
}

/**
 * @brief Joins two token with "."
 * 
 * @param cmd 
 * @param t1 
 * @param t2 
 * @return Token 
 */
Token jointoken(char* cmd, Token t1, Token t2){
    Token t3;
    strcpy(t3.lx, cmd);
    strcat(t3.lx, t1.lx);
    strcat(t3.lx, ".");
    strcat(t3.lx, t2.lx);
    return t3;
}

/**
 * @brief Get the local address of variable in method
 * 
 * @param class_name 
 * @param method_name 
 * @param symb 
 * @return int 
 */
int get_local_address(Token class_name, Token method_name, Token symb){
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].var.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].var.data[i].id.lx, symb.lx) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Get the agument variable address of method
 * 
 * @param class_name 
 * @param method_name 
 * @param symb 
 * @return int 
 */
int get_agument_address(Token class_name, Token method_name, Token symb){
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    for(int i = 0; i < symbols.data[cindx].methods.data[mindx].arguments.size; i++){
        if(strcmp(symbols.data[cindx].methods.data[mindx].arguments.data[i].id.lx, symb.lx) == 0){
            if(strcmp(symbols.data[cindx].methods.data[mindx].method_type.lx, "constructor") == 0 ||
            strcmp(symbols.data[cindx].methods.data[mindx].method_type.lx, "function") == 0){
                return i;
            }
            
            return i+1;
        }
    }
    return -1;
}

/**
 * @brief Get the field address variable from class
 * 
 * @param class_name 
 * @param symb 
 * @return int 
 */
int get_field_address(Token class_name, Token symb){
    int cindx = get_class_index(class_name);
    for(int i = 0; i < symbols.data[cindx].field_vars.size; i++){
        if(strcmp(symbols.data[cindx].field_vars.data[i].id.lx, symb.lx) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Get the static variable address from class 
 * 
 * @param class_name 
 * @param symb 
 * @return int 
 */
int get_static_address(Token class_name, Token symb){
    int cindx = get_class_index(class_name);
    for(int i = 0; i < symbols.data[cindx].static_vars.size; i++){
        if(strcmp(symbols.data[cindx].static_vars.data[i].id.lx, symb.lx) == 0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief Checks if the method is void or not
 * 
 * @param class_name 
 * @param method_name 
 * @return int 
 */
int check_method_void(Token class_name, Token method_name){
    int cindx = get_class_index(class_name);
    int mindx = get_method_index(cindx, method_name);
    return (strcmp(symbols.data[cindx].methods.data[mindx].method_return.lx, "void") == 0);
}

/**
 * @brief Removes substring from original string
 * 
 * @param string 
 * @param sub 
 */
void removeSubstr (char *string, char *sub) {
    char *match;
    int len = strlen(sub);
    while ((match = strstr(string, sub))) {
        *match = '\0';
        strcat(string, match+len);
    }
}

/**
 * @brief Compiles code into .vm file
 * 
 * @param name 
 * @return int 
 */
int compile_file(char *name){
    FILE *fp;
    removeSubstr(name, ".jack");
    strcat(name, ".vm");
    fp = fopen(name, "w+");
    if(fp == NULL){
        return 0;
    }
    for(int i = 0; i < commands.size; i++){
        fprintf(fp, "%s", commands.data[i]);
    }
    free(commands.data);
    commands.data = NULL;
    commands.size = 0;
    fclose(fp);
}
