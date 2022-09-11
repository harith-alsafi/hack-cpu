/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Harith alsafi
Student ID: 201416467 
Email: el20hzaa@leeds.ac.uk
Date Work Commenced: 01/04/2022
*************************************************************************/

#include "compiler.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int COMPILE_FLAG = 0; 

int InitCompiler ()
{
	return init_symbols();
}

ParserInfo symbol_grammer_check(char* dir_name){
	ParserInfo p;
    p.er = none;

	// write your code below
	DIR* folder = opendir(dir_name);
	struct dirent *entry;
	if(folder == NULL){
		return p;
	}

	char name[128];
	strcpy(name, dir_name);
	COMPILE_FLAG = 0;

	// reading all files 
	while (entry = readdir(folder))
	{	
		strcpy(name, dir_name);
		// checking for .jack files
		if(strstr(entry->d_name, ".jack")){
			// Parsing 
			strcat(name, "/");
			strcat(name, entry->d_name);

			add_class();
			InitParser(name);
			p = Parse();
			StopParser();
			// check symbols

			if(p.er != none){
				return p;
			} 
		}
	}
	closedir(folder);
	
	// checking the symbols 
	ParserInfo p1;
	p1 = check_symbols(dir_name);
	if(p1.er != none){
		return p1;
	}

	return p;
}

 

ParserInfo compile (char* dir_name)
{
	ParserInfo p;
    p.er = none;

	// write your code below
	DIR* folder = opendir(dir_name);
	struct dirent *entry;
	if(folder == NULL){
		return p;
	}

	// checking grammer and adding symbols
	p = symbol_grammer_check(dir_name);
	if(p.er != none){
		return p;
	}

	char name[128];
	strcpy(name, dir_name);

	// reading all files 
	COMPILE_FLAG = 1;
	FILE *fp;
	while (entry = readdir(folder))
	{	
		strcpy(name, dir_name);
		// checking for .jack files
		if(strstr(entry->d_name, ".jack")){
			// Adding directory name 
			strcat(name, "/");
			strcat(name, entry->d_name);

			// compiling 
			InitParser(name);
			p = Parse();
			StopParser();

			// printing vm file 
			compile_file(name);

		}
	}
	closedir(folder);
	return p;
}

int StopCompiler ()
{
	delete_symbols();
	return 1;
}


