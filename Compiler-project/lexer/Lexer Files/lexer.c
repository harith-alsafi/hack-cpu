/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Harith Al-Safi
Student ID: 201416467
Email: el20hzaa@leeds.ac.uk
Date Work Commenced: 14/02/2022
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE
typedef struct {
  int cl; 
  size_t size;
  Token value[];
} List;

// all tokens 
List* tokens = NULL;

// to act as an array 
int push_back(List **a, Token value) {
  size_t x = *a ? a[0]->size : 0
  , y = x + 1;

  if ((x & y) == 0) {
    void *temp = realloc(*a, sizeof **a
    + (x + y) * sizeof a[0]->value[0]);
    
    if (!temp) { 
      return 1; 
    }

    *a = temp; // or, if you like, `a[0] = temp;`
  }

  a[0]->value[x] = value;
  a[0]->size = y;
  return 0;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making 
// any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{

  return 0;
}


// Get the next token from the source file
Token GetNextToken ()
{
	Token t;
  t.tp = ERR;
  return t;
}

// peek (look) at the next token in the source file without
// removing it from the stream
Token PeekNextToken ()
{
  Token t;
  t.tp = ERR;
  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function
  Token t;
  t.tp = ERR;
  push_back(&tokens, t);
  
	return 0;
}
// do not remove the next line
#endif
