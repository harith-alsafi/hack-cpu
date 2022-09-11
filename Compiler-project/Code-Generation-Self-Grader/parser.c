#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "symbols.h"

// you can declare prototypes of parser functions below
ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatemnt();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatemnt();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo ArithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();

Token class_name;
Token method_name;
int n_method_var;
int n_method_arg;
int n_class_fields;
int whilecount;
int ifcount;
int arraymode;
int funcall;

ParserInfo classDeclar()
{
	// starting 
	ParserInfo pi;
	pi.er = none;
    Token next_token;

	// "class"
	next_token = GetNextToken();
	if(strcmp(next_token.lx, "class") != 0){
		pi.er = classExpected;
        pi.tk = next_token;
		return pi;
	}

	// ID
	next_token = GetNextToken();
	if(next_token.tp != ID){
		pi.er = idExpected;
        pi.tk = next_token;
		return pi;
	}

	// Compile vs symbol table 
	if(COMPILE_FLAG == 1){
		class_name = next_token;
		n_class_fields = get_class_fields(class_name);
	}
	else if(COMPILE_FLAG == 0){
		if(get_class_index(next_token) == -1){
			set_class_name(next_token);
		}
		else{
			pi.er = redecIdentifier;
			pi.tk = next_token;
			return pi;
		}
	}


	// "{"
	next_token = GetNextToken();
	if(strcmp(next_token.lx, "{") == 0){


		// memberDeclar 
		next_token = PeekNextToken();
		while(strcmp(next_token.lx, "static") == 0 ||
		strcmp(next_token.lx, "field") == 0 ||
		strcmp(next_token.lx, "constructor") == 0 ||
		strcmp(next_token.lx, "function") == 0 || 
		strcmp(next_token.lx, "method") == 0){
			pi = memberDeclar();
			if(pi.er != none){
				return pi;
			}
			next_token = PeekNextToken();
		}
		
		// "}"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "}") != 0){
            pi.tk = next_token;
			pi.er = closeBraceExpected;
		}
	}
	else{
        pi.tk = next_token;
		pi.er = openBraceExpected;
	}
	return pi;
}

ParserInfo memberDeclar(){
	// starting 
	ParserInfo pi;
	pi.er = none;
    Token next_token;
	

	next_token= PeekNextToken();
	// classVarDeclar
	if(strcmp(next_token.lx, "static") == 0 || 
	strcmp(next_token.lx, "field") == 0){
		pi = classVarDeclar();
	}

	// subroutineDeclar
	else if(strcmp(next_token.lx, "constructor") == 0 || 
	strcmp(next_token.lx, "function") == 0 || 
	strcmp(next_token.lx, "method") == 0){
		pi = subroutineDeclar();
	}
	else{
		pi.er = memberDeclarErr;
		pi.tk = next_token;
	} 
	return pi;
}

ParserInfo classVarDeclar(){
	// starting 
	ParserInfo pi;
	pi.er = none;
    Token next_token;

	next_token = GetNextToken();
	
	// "(static | field)""
	if(strcmp(next_token.lx, "static") == 0 || 
	strcmp(next_token.lx, "field") == 0){
		Token kind = next_token;
		// "type"
		Token tpe = PeekNextToken();
		pi = type();
		if(pi.er == none){
			next_token = GetNextToken();
			// "ID"
			if(next_token.tp != ID){
				pi.er = idExpected;
				pi.tk = next_token;
				return pi;
			}

			if(COMPILE_FLAG == 0){
				// variable doesnt exist 
				if(check_current_class_scope(next_token) == 0){
					add_var_to_class(kind, tpe, next_token);
				}
				// variable exists 
				else{
					pi.er = redecIdentifier;
					pi.tk = next_token;
					return pi;
				}
			}

			
			next_token = PeekNextToken();
			while (strcmp(next_token.lx, ",") == 0)
			{
				GetNextToken();
				next_token = PeekNextToken();

				// ","
				if(next_token.tp != ID){
					pi.er = idExpected;
					pi.tk = next_token;
					return pi;
				}
				else{
					GetNextToken();
					if(COMPILE_FLAG == 0){
						// variable doesnt exist 
						if(check_current_class_scope(next_token) == 0){
							add_var_to_class(kind, tpe, next_token);
						}
						// variable exists 
						else{
							pi.er = redecIdentifier;
							pi.tk = next_token;
							return pi;
						}
					}
				}	
				next_token = PeekNextToken();
			}

			// ";"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, ";") != 0){
				pi.er = semicolonExpected;
				pi.tk = next_token;
				return pi;
			}
		}
	}
	else{
		pi.er = classVarErr;
		pi.tk = next_token;
	}
	if(pi.tk.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo type(){	
	ParserInfo pi;
	pi.er = none;
    Token next_token;

	next_token = GetNextToken();
	if(strcmp(next_token.lx, "int") != 0 && 
	strcmp(next_token.lx, "char") != 0 &&
	strcmp(next_token.lx, "boolean") != 0 &&
	next_token.tp != ID){
		pi.er = illegalType;
		pi.tk = next_token;
		return pi;
	}

	if(pi.tk.tp == ERR){
		pi.tk = next_token;
		pi.er = lexerErr;
	}
	return pi;
}

ParserInfo subroutineDeclar(){
	ParserInfo pi;
	pi.er = none;
    Token next_token;

	next_token = GetNextToken();
	// "(constructor | function | method)"
	if(strcmp(next_token.lx, "constructor") == 0 || 
	strcmp(next_token.lx, "function") == 0 || 
	strcmp(next_token.lx, "method") == 0){
		Token method_type = next_token;
		// "(void | type)"
		next_token = PeekNextToken();
		Token tpe = next_token;
		if(strcmp(next_token.lx, "void") == 0){
			GetNextToken();
		}
		else{
			pi = type();
			if(pi.er != none){
				return pi;
			}
		}

		// "ID"
		next_token = GetNextToken();
		if(next_token.tp != ID){
			pi.er = idExpected;
			pi.tk = next_token;
			return pi;
		}

		if(COMPILE_FLAG == 1){
			method_name = next_token;
			n_method_arg = get_method_args(class_name, method_name);
			n_method_var = get_method_locals(class_name, method_name);
			add_command(
				jointoken("function ", class_name, method_name).lx,
				n_method_var, 1
			);	
			if(strcmp(method_type.lx, "constructor") == 0){
				add_command("push constant", n_class_fields, 1);
				add_command("call Memory.alloc", 1, 1);
				add_command("pop pointer", 0, 1);
			}
			else if(strcmp(method_type.lx, "method") == 0){
				add_command("push argument", 0, 1);
				add_command("pop pointer", 0, 1);
			}
			ifcount = -1;
			whilecount = -1;
		}
		else if(COMPILE_FLAG == 0){
			// method deosnt exist
			if(check_current_class_methods(next_token) == 0){
				add_method_to_class(method_type, tpe, next_token);
			}
			// method does exist
			else{
				pi.er = redecIdentifier;
				pi.tk = next_token;
				return pi;
			}
		}

		// "("
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "(") == 0){

			// "paramList"
			pi = paramList();
			if(pi.er != none){
				return pi;
			}

			// ")"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, ")") == 0){
				// "subroutineBody"
				pi = subroutineBody();
				if(pi.er != none){
					return pi;
				}
			}

			else{
				pi.er = closeParenExpected;
				pi.tk = next_token;
			}
		}
		else{
			pi.er = openParenExpected;
			pi.tk = next_token;
		}	
	}
	else{
		pi.er = subroutineDeclarErr;
		pi.tk = next_token;
	}
	if(pi.tk.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}



ParserInfo paramList(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	// "empty"
	next_token = PeekNextToken();
	if(strcmp(next_token.lx, "int") != 0 && 
	strcmp(next_token.lx, "char") != 0 &&
	strcmp(next_token.lx, "boolean") != 0 &&
	next_token.tp != ID){
		return pi;
	}


	// "type"
	Token tpe = PeekNextToken();
	pi = type();
	if(pi.er != none){
		return pi;
	}

	// ID
	next_token = GetNextToken();
	if(next_token.tp != ID){
		pi.er = idExpected;
		pi.tk = next_token;
		return pi;
	}

	if(COMPILE_FLAG == 0){
		// arg doesnt exist 
		if(check_current_method_scope(next_token) == 0){
			add_arg_to_method(tpe, next_token);
		}
		// arg exists 
		else{
			pi.er = redecIdentifier;
			pi.tk = next_token;
			return pi;
		}
	}

	// "," 
	next_token = PeekNextToken();
	while(strcmp(next_token.lx, ",") == 0){
		GetNextToken();

		// "type"
		Token tpe = PeekNextToken();
		pi = type();
		if(pi.er != none){
			return pi;
		}	

		// "ID"
		next_token = GetNextToken();
		if(next_token.tp != ID){
			pi.er = idExpected;
			return pi;
		}
		if(COMPILE_FLAG == 0){
			// arg doesnt exist 
			if(check_current_method_scope(next_token) == 0){
				add_arg_to_method(tpe, next_token);
			}
			// arg exists 
			else{
				pi.er = redecIdentifier;
				pi.tk = next_token;
				return pi;
			}
		}

		next_token = PeekNextToken(); 
	}

	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo subroutineBody(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	//"{"
	next_token = GetNextToken();
	if(strcmp(next_token.lx, "{") == 0){
		// no statement
		next_token = PeekNextToken();
		if(strcmp(next_token.lx, "}") == 0){
			GetNextToken();
			pi.er = none;
			return pi;
		}

		// "statement"
		next_token = PeekNextToken();
		while (strcmp(next_token.lx, "var") == 0 || 
		strcmp(next_token.lx, "let") == 0 || 
		strcmp(next_token.lx, "if") == 0 || 
		strcmp(next_token.lx, "while") == 0 ||
		strcmp(next_token.lx, "do") == 0 ||
		strcmp(next_token.lx, "return") == 0)
		{
			pi = statement();
			if(pi.er != none){
				return pi;
			}
			next_token = PeekNextToken();
		}

		// "}"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "}") != 0){
			pi.er = closeBraceExpected;
			pi.tk = next_token;
		}

	}
	else{
		pi.er = openBraceExpected;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo statement(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	next_token = PeekNextToken();
	
	// varDeclarStatement
	if(strcmp(next_token.lx, "var") == 0){
		return varDeclarStatement();
	}
	// letStatement
	else if(strcmp(next_token.lx, "let") == 0){
		return letStatemnt();
	}
	// ifStatement
	else if(strcmp(next_token.lx, "if") == 0){
		return ifStatement();
	}
	// whileStatement
	else if(strcmp(next_token.lx, "while") == 0){
		return whileStatement();
	}
	// doStatement 
	else if(strcmp(next_token.lx, "do") == 0){
		return doStatement();
	}
	// returnStatement 
	else if(strcmp(next_token.lx, "return") == 0){
		return returnStatemnt();
	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo varDeclarStatement(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;


	// "var"
	next_token = GetNextToken();
	if(strcmp(next_token.lx, "var") == 0){
		// type 
		Token tpe = PeekNextToken();
		pi = type();
		if(pi.er != none){
			return pi;
		}
		
		// "ID"
		next_token = GetNextToken();
		if(next_token.tp != ID){
			pi.er = idExpected;
			pi.tk = next_token;
			return pi;
		}

		if(COMPILE_FLAG == 0){
			// var doesnt exist 
			if(check_current_method_scope(next_token) == 0){
				add_var_to_method(tpe, next_token);
			}
			// var exists 
			else{
				pi.er = redecIdentifier;
				pi.tk = next_token;
				return pi;
			}
		}

		next_token = PeekNextToken();
		while (strcmp(next_token.lx, ",") == 0)
		{
			GetNextToken();
			next_token = PeekNextToken();

			// ","
			if(next_token.tp != ID){
				pi.er = idExpected;
				pi.tk = next_token;
				return pi;
			}
			else{
				GetNextToken();
				if(COMPILE_FLAG == 0){
					// var doesnt exist 
					if(check_current_method_scope(next_token) == 0){
						add_var_to_method(tpe, next_token);
					}
					// var exists 
					else{
						pi.er = redecIdentifier;
						pi.tk = next_token;
						return pi;
					}
				}
			}	
			next_token = PeekNextToken();
		}

		// ";"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, ";") != 0){
			pi.er = semicolonExpected;
			pi.tk = next_token;
			return pi;
		}
	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo letStatemnt(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;
	funcall = 0;
	arraymode = 0;
	next_token = GetNextToken();
	int iargument = -1;
	int ifield = -1;
	int ilocal = -1;
	int istatic = -1;
	// "let"
	if(strcmp(next_token.lx, "let") == 0){
		// "ID"
		next_token = GetNextToken();
		if(next_token.tp != ID){
			pi.er = idExpected;
			pi.tk = next_token;
			return pi;
		}

		if(COMPILE_FLAG == 1){
			ilocal = get_local_address(class_name, method_name, next_token);
			iargument = get_agument_address(class_name, method_name, next_token);
			ifield = get_field_address(class_name, next_token);
			istatic = get_static_address(class_name, next_token);
		}


		// "["
		next_token = PeekNextToken();
		if(strcmp(next_token.lx, "[") == 0){
			arraymode = 1;
			GetNextToken();
			pi = expression();
			if(pi.er != none){
				return pi;
			}

			if(COMPILE_FLAG == 1){
				if(iargument != -1){
					add_command("push argument", iargument, 1);
				}
				else if(ilocal != -1){
					add_command("push local", ilocal, 1);
				}
				else if(ifield != -1){
					add_command("push this", ifield, 1);
				}
				else if(istatic != -1){
					add_command("push static", istatic, 1);
				}
			}
			// "]"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, "]") != 0){
				pi.er = closeBracketExpected;
				pi.tk = next_token;
				return pi;
			}
			if(COMPILE_FLAG == 1){
				add_command("add", -1, 0);
			}
		}

		// "="
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "=") != 0){
			pi.er = equalExpected;
			pi.tk = next_token;
			return pi;
		}

		// "expression"
		pi = expression();
		if(pi.er != none){
			return pi;
		} 

		// ";"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, ";") != 0){
			pi.er = semicolonExpected;
			pi.tk = next_token;
		}

		if(COMPILE_FLAG == 1){
		
			if(arraymode == 0){
				if(iargument != -1){
					add_command("pop argument", iargument, 1);
				}
				else if(ilocal != -1){
					add_command("pop local", ilocal, 1);
				}
				else if(ifield != -1){
					add_command("pop this", ifield, 1);
				}
				else if(istatic != -1){
					add_command("pop static", istatic, 1);
				}
			}
			else{
				if(funcall){
					add_command("pop temp", 0, 1);
				}
				add_command("pop pointer", 1, 1);
				if(funcall){
					add_command("push temp", 0, 1);
				}
				add_command("pop that", 0, 1);
			}

		}

	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo ifStatement(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	next_token = GetNextToken();
	// "if"
	if(strcmp(next_token.lx, "if") == 0){
		if(COMPILE_FLAG == 1){
			ifcount++;
		}
		// "("
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "(") == 0){
			// expression 
			pi = expression();
			if(pi.er != none){
				return pi;
			}
			// ")"
			next_token = GetNextToken();

			if(COMPILE_FLAG == 1){
				add_command("if-goto IF_TRUE", ifcount, 0);
				add_command("goto IF_FALSE", ifcount, 0);
				add_command("label IF_TRUE", ifcount, 0);
			}

			if(strcmp(next_token.lx, ")") == 0){
				// "{"
				next_token = GetNextToken();
				if(strcmp(next_token.lx, "{") == 0){

					// "statement"
					next_token = PeekNextToken();
					while (strcmp(next_token.lx, "var") == 0 || 
					strcmp(next_token.lx, "let") == 0 || 
					strcmp(next_token.lx, "if") == 0 || 
					strcmp(next_token.lx, "while") == 0 ||
					strcmp(next_token.lx, "do") == 0 ||
					strcmp(next_token.lx, "return") == 0)
					{
						pi = statement();
						if(pi.er != none){
							return pi;
						}
						next_token = PeekNextToken();
					}
					 
					// "}"
					next_token = GetNextToken();
					if(strcmp(next_token.lx, "}") == 0){
						


						// "else" 
						next_token = PeekNextToken();
						if(COMPILE_FLAG == 1){
							if(strcmp(next_token.lx, "else") == 0){
								add_command("goto IF_END", ifcount, 0);
							}
							add_command("label IF_FALSE", ifcount, 0);
						}
						if(strcmp(next_token.lx, "else") == 0){

							GetNextToken();
							next_token = GetNextToken();
							// "{"
							if(strcmp(next_token.lx, "{") == 0){
								// "statement"
								next_token = PeekNextToken();
								while (strcmp(next_token.lx, "var") == 0 || 
								strcmp(next_token.lx, "let") == 0 || 
								strcmp(next_token.lx, "if") == 0 || 
								strcmp(next_token.lx, "while") == 0 ||
								strcmp(next_token.lx, "do") == 0 ||
								strcmp(next_token.lx, "return") == 0)
								{
									pi = statement();
									if(pi.er != none){
										return pi;
									}
									next_token = PeekNextToken();
								}

								// "}"
								next_token = GetNextToken();
								if(strcmp(next_token.lx, "}") != 0){
									pi.er = closeBraceExpected;
									pi.tk = next_token;
								}

								if(COMPILE_FLAG == 1){
									add_command("label IF_END", ifcount, 0);
								}
							}
							else{
								pi.er = openBraceExpected;
								pi.tk = next_token;
							}
						}
					}
					else{
						pi.er = closeBraceExpected;
						pi.tk = next_token;
					}
				}
				else{
					pi.er = openBraceExpected;
					pi.tk = next_token;
				}
			}
			else{
				pi.er = closeParenExpected;
				pi.tk = next_token;
			}
		}
		else{
			pi.er = openParenExpected;
			pi.tk = next_token;
		}
	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo whileStatement(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	next_token = GetNextToken();
	// "while"
	if(strcmp(next_token.lx, "while") == 0){
		if(COMPILE_FLAG == 1){
			whilecount++;
			add_command("label WHILE_EXP", whilecount, 0);
		}

		// "("
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "(") == 0){
			// "expression"
			pi = expression();
			if(pi.er != none){
				return pi;
			}

			// ")"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, ")") == 0){	
				if(COMPILE_FLAG == 1){
					add_command("not", -1, 0);
					add_command("if-goto WHILE_END", whilecount, 0);
				}


				//"{"
				next_token = GetNextToken();
				if(strcmp(next_token.lx, "{") == 0){	
					// "statement"
					next_token = PeekNextToken();
					while (strcmp(next_token.lx, "var") == 0 || 
					strcmp(next_token.lx, "let") == 0 || 
					strcmp(next_token.lx, "if") == 0 || 
					strcmp(next_token.lx, "while") == 0 ||
					strcmp(next_token.lx, "do") == 0)
					{
						pi = statement();
						if(pi.er != none){
							return pi;
						}
						next_token = PeekNextToken();
					}

					// "}"
					next_token = GetNextToken();
					if(strcmp(next_token.lx, "}") != 0){	
						pi.er = closeBraceExpected;
						pi.tk = next_token;
					}
					if(COMPILE_FLAG == 1){
						add_command("goto WHILE_EXP", whilecount, 0);
						add_command("label WHILE_END", whilecount, 0);
					}
				}
				else{
					pi.er = openBraceExpected;
					pi.tk = next_token;
				}
			}	
			else{
				pi.er = closeParenExpected;
				pi.tk = next_token;
			}
		}
		else{
			pi.er = openParenExpected;
			pi.tk = next_token;
		}
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo doStatement(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	next_token = GetNextToken();
	// "do"
	if(strcmp(next_token.lx, "do") == 0){

		// "subroutineCall"
		pi = subroutineCall();
		if(pi.er != none){
			return pi;
		}

		// ";"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, ";") != 0){
			pi.er = semicolonExpected;
			pi.tk = next_token;
		}
	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo subroutineCall(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	next_token = GetNextToken();

	Token cname;
	Token mname;
	Token vname;
	int indx;
	int localcall;




	// ID
	if(next_token.tp == ID){
		
		if(COMPILE_FLAG == 1){
			indx = get_type_index(class_name);
			cname = class_name;
			mname = next_token;
			localcall = 1;
		}
		// "."
		next_token = PeekNextToken();
		if(strcmp(next_token.lx, ".") == 0){
			GetNextToken();

			next_token = GetNextToken();
			// ID
			if(next_token.tp != ID){
				pi.er = idExpected;
				pi.tk = next_token;
				return pi;
			}
			if(COMPILE_FLAG == 1){
				indx = get_type_index(mname);
				vname = mname;
				cname = get_class_name(class_name, method_name, mname);
				mname = next_token;
				localcall = 0;
			}
		}



		// "("
		next_token = GetNextToken();
		if(strcmp(next_token.lx, "(") == 0){

			if(COMPILE_FLAG == 1){
				if(indx < 0 && localcall == 0){
					int ilocal = get_local_address(class_name, method_name, vname);
					int iargument = get_agument_address(class_name, method_name, vname);
					int ifield = get_field_address(class_name, vname);
					int istatic = get_static_address(class_name, vname);
					if(iargument != -1){
						add_command("push argument", iargument, 1);
					}
					else if(ilocal != -1){
						add_command("push local", ilocal, 1);
					}
					else if(ifield != -1){
						add_command("push this", ifield, 1);
					}
					else if(istatic != -1){
						add_command("push static", istatic, 1);
					}
				}
			}

			if(COMPILE_FLAG == 1){
				if(localcall == 1){
					add_command("push pointer", 0, 1);
				}
			}


			// "expressionList"
			pi = expressionList();
			if(pi.er != none){
				return pi;
			}

			// ")"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, ")") != 0){
				pi.er = closeParenExpected;
				pi.tk = next_token;
			}

			if(COMPILE_FLAG == 1){
				if(indx >= 0 && localcall == 0){
					add_command(
						jointoken("call ", cname, mname).lx,
					get_method_args(cname, mname), 1);					
				}
				else{

					add_command(
						jointoken("call ", cname, mname).lx,
					get_method_args(cname, mname)+1, 1);
				}
				if(check_method_void(cname, mname)){
					add_command("pop temp", 0, 1);
				}
			}
		}
		else{
			pi.er = openParenExpected;
			pi.tk = next_token;
		}
	}
	else{
		pi.tk = next_token;
		pi.er = idExpected;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}



ParserInfo expressionList(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	// empty	
	next_token = PeekNextToken();
	if(strcmp(next_token.lx, ")") == 0){
		return pi;
	}

	// "expression" 
	pi = expression();
	if(pi.er != none){
		return pi;
	}

	// ","
	next_token = PeekNextToken();
	while(strcmp(next_token.lx, ",") == 0)
	{
		GetNextToken();

		pi = expression();
		if(pi.er != none){
			return pi;
		}
		
		next_token = PeekNextToken();
	}



	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo returnStatemnt(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	// "return"
	next_token = GetNextToken();
	if(strcmp(next_token.lx, "return") == 0){
		next_token = PeekNextToken();
		// "expression"
		if(strcmp(next_token.lx, "-") == 0 || strcmp(next_token.lx, "~") == 0 || 
		strcmp(next_token.lx, "*") == 0 || strcmp(next_token.lx, "/") == 0 ||
		next_token.tp == INT || next_token.tp == STRING || next_token.tp == ID || 
		strcmp(next_token.lx, "true") == 0 || strcmp(next_token.lx, "false") == 0 ||
		strcmp(next_token.lx, "null") == 0 || strcmp(next_token.lx, "this") == 0 ||
		strcmp(next_token.lx, "(") == 0){
			pi = expression();
			if(pi.er != none){
				return pi;
			}
		}
		else{
			if(COMPILE_FLAG == 1){
				add_command("push constant", 0, 1);
			}
		}
		// ";"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, ";") != 0){
			pi.er = semicolonExpected;
			pi.tk = next_token;
		}
		if(COMPILE_FLAG == 1){
			add_command("return", -1, 1);
		}
	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo expression(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	// "relationalExpression"
	pi = relationalExpression();
	if(pi.er != none){
		return pi;
	}

	// 
	next_token = PeekNextToken();
	while(strcmp(next_token.lx, "&") == 0 || strcmp(next_token.lx, "|") == 0)
	{
		GetNextToken();

		// "relationalExpression"
		pi = relationalExpression();
		if(pi.er != none){
			return pi;
		}

		if(COMPILE_FLAG == 1){
			if (strcmp(next_token.lx, "&") == 0){
				add_command("and", -1, 1);
			}
			else if(strcmp(next_token.lx, "|") == 0){
				add_command("or", -1, 1);
			}
		}

		next_token = PeekNextToken();
	}

	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo relationalExpression(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;


	pi = ArithmeticExpression();
	if(pi.er != none){
		return pi;
	}

	next_token = PeekNextToken();
	while(strcmp(next_token.lx, "=") == 0 || strcmp(next_token.lx, "<") == 0 ||
	strcmp(next_token.lx, ">") == 0)
	{
	
		GetNextToken();

		// "ArithmeticExpression"
		pi = ArithmeticExpression();
		if(pi.er != none){
			return pi;
		}
		if(COMPILE_FLAG == 1){
			if (strcmp(next_token.lx, "<") == 0){
				add_command("lt", -1, 1);
			}
			else if(strcmp(next_token.lx, ">") == 0){
				add_command("gt", -1, 1);
			}
			else if(strcmp(next_token.lx, "=") == 0){
				add_command("eq", -1, 1);
			}
		}

		next_token = PeekNextToken();
	}

	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo ArithmeticExpression(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;
	
	pi = term();
	if(pi.er != none){
		return pi;
	}

	next_token = PeekNextToken();
	while(strcmp(next_token.lx, "+") == 0 || strcmp(next_token.lx, "-") == 0)
	{
	
		GetNextToken();

		// "term"
		pi = term();
		if(pi.er != none){
			return pi;
		}
		if(COMPILE_FLAG == 1){
			if (strcmp(next_token.lx, "+") == 0){
				add_command("add", -1, 1);
			}
			else if(strcmp(next_token.lx, "-") == 0){
				add_command("sub", -1, 1);
			}
		}
		next_token = PeekNextToken();
	}

	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo term(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;

	// "factor"
	pi = factor();
	if(pi.er != none){
		return pi;
	}

	
	next_token = PeekNextToken();
	while(strcmp(next_token.lx, "*") == 0 || strcmp(next_token.lx, "/") == 0)
	{
		GetNextToken();

		// "factor"
		pi = factor();
		if(pi.er != none){
			return pi;
		}

		if(COMPILE_FLAG == 1){
			if (strcmp(next_token.lx, "*") == 0){
				add_command("call Math.multiply", 2, 1);
				funcall = 1;
			}
			else if(strcmp(next_token.lx, "/") == 0){
				add_command("call Math.divide", 2, 1);
				funcall = 1;
			}
		}

		next_token = PeekNextToken();
		
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

ParserInfo factor(){
	Token next_token;
	ParserInfo pi;

	next_token = PeekNextToken();
	if(strcmp(next_token.lx, "-") == 0 || strcmp(next_token.lx, "~") == 0){
		GetNextToken();

		pi = operand();

		if(COMPILE_FLAG == 1){
			if (strcmp(next_token.lx, "-") == 0){
				add_command("neg", -1, 1);
			}
			else if(strcmp(next_token.lx, "~") == 0){
				add_command("not", -1, 1);
			}
		}
		return pi;
	}

	return operand();
}

ParserInfo operand(){
	ParserInfo pi;
	pi.er = none;
	Token next_token;
	int iargument = -1;
	int ilocal = -1;
	int ifield = -1;
	int istatic = -1;
	Token cname;
	Token mname;
	int localcall;
	int indx;

	next_token = GetNextToken();
	// INT | STRING | "true" | "false" | "null" | "this" 
	if(next_token.tp == INT || next_token.tp == STRING || 
	strcmp(next_token.lx, "true") == 0 || strcmp(next_token.lx, "false") == 0 ||
	strcmp(next_token.lx, "null") == 0 || strcmp(next_token.lx, "this") == 0){
		pi.er = none;
		if(COMPILE_FLAG == 1){
			if(next_token.tp == INT){
				add_command("push constant", atoi(next_token.lx), 1);
			}
			else if(strcmp(next_token.lx, "true") == 0){
				add_command("push constant", 0, 1);
				add_command("not", -1, 1);
			}
			else if(strcmp(next_token.lx, "false") == 0){
				add_command("push constant", 0, 1);
			}
			else if(strcmp(next_token.lx, "this") == 0){
				add_command("push pointer", 0, 1);
			}
			else if(next_token.tp == STRING){
				add_command("push constant", strlen(next_token.lx), 1);
				add_command("call String.new", 1, 1);
				for(int i = 0; i < strlen(next_token.lx); i++){
					add_command("push constant", next_token.lx[i], 1);
					add_command("call String.appendChar", 2, 1);
				}
				funcall = 1;
			}
			else if(strcmp(next_token.lx, "null") == 0){
				add_command("push constant", 0, 1);
			}
		}
	}
	// ID


	else if(next_token.tp == ID){
		Token prv = next_token;
		if(COMPILE_FLAG == 1){
			ilocal = get_local_address(class_name, method_name, next_token);
			iargument = get_agument_address(class_name, method_name, next_token);
			ifield = get_field_address(class_name, next_token);
			istatic = get_static_address(class_name, next_token);
			indx = get_type_index(class_name);
			cname = class_name;
			mname = next_token;
			localcall = 1;
		}

		// "."
		next_token = PeekNextToken();
		if(strcmp(next_token.lx, ".") == 0){
			GetNextToken();
			next_token = GetNextToken();
			// ID
			if(next_token.tp != ID){
				pi.er = idExpected;
				pi.tk = next_token;
				return pi;
			}
			if(COMPILE_FLAG == 1){
				indx = get_type_index(mname);
				cname = get_class_name(class_name, method_name, mname);
				mname = next_token;
				localcall = 0;
			}
		}


		

		// "["
		next_token = PeekNextToken();
		if(strcmp(next_token.lx, "[") != 0 && COMPILE_FLAG == 1){
			if(iargument != -1){
				add_command("push argument", iargument, 1);
			}
			else if(ilocal != -1){
				add_command("push local", ilocal, 1);
			}
			else if(ifield != -1){
				add_command("push this", ifield, 1);
			}
			else if(istatic != -1){
				add_command("push static", istatic, 1);
			}
		}
		if(strcmp(next_token.lx, "[") == 0){
			GetNextToken();

			// "expression"
			pi = expression();
			if(pi.er != none){
				return pi;
			}

			if(COMPILE_FLAG == 1){
				if(iargument != -1){
					add_command("push argument", iargument, 1);
				}
				else if(ilocal != -1){
					add_command("push local", ilocal, 1);
				}
				else if(ifield != -1){
					add_command("push this", ifield, 1);
				}
				else if(istatic != -1){
					add_command("push static", istatic, 1);
				}
			}

			// "]"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, "]") != 0){
				pi.er = closeBracketExpected;
				pi.tk = next_token;
				return pi;
			}

			if(COMPILE_FLAG == 1){
				add_command("add", -1, 1);
				add_command("pop pointer", 1, 1);
				add_command("push that", 0, 1);
			}

		}

		// "("
		else if(strcmp(next_token.lx, "(") == 0){
			GetNextToken();
			// "expressionList"
			pi = expressionList();
			if(pi.er != none){
				return pi;
			}

			// ")"
			next_token = GetNextToken();
			if(strcmp(next_token.lx, ")") != 0){
				pi.er = closeParenExpected;
				pi.tk = next_token;
				return pi;
			}	
			if(COMPILE_FLAG == 1){
				funcall = 1;

				if(indx >= 0 && localcall == 0){
					add_command(
						jointoken("call ", cname, mname).lx,
					get_method_args(cname, mname), 1);	
				}
				else{
					if(localcall == 1){
						add_command("push pointer", 0, 1);
					}
					add_command(
						jointoken("call ", cname, mname).lx,
					get_method_args(cname, mname)+1, 1);
					
				}
				if(check_method_void(cname, mname)){
					add_command("pop temp", 0, 1);
				}
			}
		}
	}
	// "("
	else if(strcmp(next_token.lx, "(") == 0){
		// "expression"
		pi = expression();
		if(pi.er != none){
			return pi;
		}
		
		// ")"
		next_token = GetNextToken();
		if(strcmp(next_token.lx, ")") != 0){
			pi.er = closeParenExpected;
			pi.tk = next_token;
			
		}

	}
	else{
		pi.er = syntaxError;
		pi.tk = next_token;
	}
	if(next_token.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next_token;
	}
	return pi;
}

int InitParser (char* file_name)
{
	return InitLexer(file_name);
}

ParserInfo Parse ()
{

	ParserInfo pi;
	pi.er = none;
	whilecount = -1;
	ifcount = -1;
	arraymode = 0;
	funcall = 0;

	Token next = PeekNextToken();
	while (next.tp != EOFile)
	{
		pi = classDeclar();
		if(pi.er != none){
			break;
		}
		next = GetNextToken();
	}
	
	if(next.tp == ERR){
		pi.er = lexerErr;
		pi.tk = next;
	}
	return pi;
}


int StopParser ()
{
	return StopLexer();
}

// #ifndef TEST_PARSER
// int main ()
// {

// 	return 1;
// }
// #endif
