#include <stdio.h>
#include <stdlib.h>

typedef enum {RESWORD, ID , INT , SYMBOL, STRING, EOFile, ERR} TokenType;

// a structure for representing tokens
typedef struct {
  TokenType tp;		// the type of this token, e.g. INT
  char lx[128];		// the lexeme (string) of the token, e.g. "34". If the lexer encounters an error this string is set to an error message
  int ec;			// If the lexer encounters an error this value is set to the proper error code (see the above enumerated list of errors)
  int ln;			// the line number of the source file where the token exists
  char fl[32];		// the file (name) in which this token exists
} Token;

typedef struct {
  size_t size;
  Token value[];
} List;


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

int main(int argc, char const *argv[])
{
  List *a = NULL;
  
  Token tk1; 

  tk1.tp = ERR;
  push_back(&a, tk1);

  tk1.tp = RESWORD;
  push_back(&a, tk1);

  tk1.tp = ID;
  push_back(&a, tk1);

  tk1.tp = SYMBOL;
  push_back(&a, tk1);

  tk1.tp = STRING;
  push_back(&a, tk1);

  printf("%ld \t %d \n", a->size, a->value[0].tp);  
  printf("%ld \t %d \n", a->size, a->value[1].tp);  
  printf("%ld \t %d \n", a->size, a->value[2].tp);
  printf("%ld \t %d \n", a->size, a->value[3].tp);
  printf("%ld \t %d \n", a->size, a->value[4].tp);  



  
  return 0;
}




