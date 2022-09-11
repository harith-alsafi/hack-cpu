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

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE
typedef struct {
  int cl; 
  size_t size;
  Token value[];
} List;

typedef enum{
  comment_state,
  running_state,
  string_state,
  quit_eofstr,
  quit_nwlstr,
  hold_state,
  quit_ills
} State;

// all tokens 
List* tokens = NULL;

/**
 * @brief Adds a new element to a List struct 
 * Returns 1 for success
 * 
 * @param list 
 * @param value 
 * @return int 
 */
int push_back(List **list, Token value) {
  size_t x = *list ? list[0]->size : 0, y = x + 1;

  if ((x & y) == 0) {
	void *temp = realloc(*list, sizeof **list
	+ (x + y) * sizeof list[0]->value[0]);
	
	if (!temp) { 
	  return 1; 
	}

	*list = temp; 
  }

  list[0]->value[x] = value;
  list[0]->size = y;
  return 0;
}

/**
 * @brief Prints all tokens 
 * 
 */
void print_tokens(){
  for(int i = 0; i < tokens->size; i++){
	printf("<%s, %d, %s, %d> \n", tokens->value[i].fl,
	tokens->value[i].ln, tokens->value[i].lx,
	tokens->value[i].tp);
  }
}

/**
 * @brief Gets the index of where a substring starts in main string
 * Returns -1 if failure 
 * 
 * @param str 
 * @param sub
 * @return int 
 */
int fndstr(char *str, const char *sub){
  char *result = strstr(str, sub);
  // checking if we have symbol 
  if (result != NULL){
	return result-str;
  }
  return -1;
}

/**
 * @brief Removes sub string from string at first occurance
 * Returns 1 if success
 * 
 * @param str 
 * @param sub 
 * @return int
 */
int strrmv(char * str, const char * sub)
{
  int i, j = 0;
  int len, removeLen;
  int found = 0;

  len = strlen(str);
  removeLen = strlen(sub);
  for(i=0; i<len; i++){
	found = 1;
	for(j=0; j<removeLen; j++){
	  if(str[i+j] != sub[j]){
		found = 0;
		break;
	  }
	}

	// If word has been found then remove it by shifting characters
	if(found == 1){
	  for(j=i; j<= (len-removeLen); j++){
		str[j] = str[j + removeLen];
	  }

	  // Terminate from loop so only first occurrence is removed
	  return 1;
	}
  }
  return 0;
}

/**
 * @brief Removes first occurance of character 
 * 
 * @param str 
 * @param c 
 */
void chremv(char *str, const char c)
{
	int i,temp=1,n;
	n = strlen(str); 
	 
	for(i = 0; i < n; i++)  { 
	  if(temp){
		if(c==str[i]){
		  temp=0;
		  str[i]=str[i+1];
		}
	  }
	  else{
		str[i]=str[i+1];
	  }
	}
}

/**
 * @brief Removes white space from string 
 * 
 * @param str 
 */
void rmvspc(char* str) {
	char* tmp = str;
	do {
	  while(isspace(*str)){
		str++;
	  }  
	} while(*tmp++ = *str++);
}

/**
 * @brief Slices the string and stores it into buffer
 * 
 * @param str given string 
 * @param buffer string to store
 * @param start start index
 * @param end end index
 */
void strslc(const char * str, char * buffer, int start, int end)
{
	int j = 0;
	for (int i = start; i <= end; ++i ) {
	  buffer[j++] = str[i];
	}
	buffer[j] = 0;
}

/**
 * @brief Returns number of occurance of a character 
 * 
 * @param str
 * @param c 
 * @return int 
 */
int modstr(char *str, char c){
  int sum = 0;
  for(int i = 0; i < strlen(str); i++){
	if(str[i] == c){
	  sum++;
	}
  }
  return sum; 
}

/**
 * @brief Returns an array of all indexes for an occurance of a character
 * 
 * @param str 
 * @param c 
 * @return int* 
 */
int* findch(char *str, char c){
  if(modstr(str, c) == 0){
	return NULL;
  }
  else{
	int* indx = malloc(modstr(str, c));
	int j = 0;
	for(int i = 0; i < strlen(str); i++){
	  if(str[i] == c){
		indx[j] = i;
		j++;
	  }
	}
	return indx;
  }
}

/**
 * @brief Removes all elements of a string until we reach the given character (we also remove that character)
 * 
 * @param str 
 * @param c 
 */
void strers(char *str, char c){
  if(modstr(str, c) == 0){
	return;
  }
  else{
	int len = strlen(str);
	for(int i = 0; i < len; i++){
	  if(str[0] == c){
		chremv(str, str[0]);
		return;
	  }
	  else{
		chremv(str, str[0]);
	  }
	}
  }
}

// reserved words
const int nres = 21;
char* res[21]= {
  "class", "constructor", "method", "function",
  "int", "boolean", "char", "void", "var", "static", 
  "field", "let", "do", "if", "else","while",
  "return", "true", "false", "null", "this"
};

// symbols 
const int nsym = 19;
char *sym[19] = {
  "(", ")", "[", "]",
  "{", "}", ",", ";", "=", ".", 
  "+", "-", "*", "/", "&","|",
  "~", "<", ">"
};

/**
 * @brief Used to check if a string contains a symbol 
 * We remove the first occurance of the symbol from string
 * Will only happen if the string starts with that symbol
 * returns the removed symbol
 * 
 * @param str 
 * @return char* 
 */
char* check_sym(char *str){
  for(int i = 0; i < nsym; i++){
	if(fndstr(str, sym[i]) == 0){
	  strrmv(str, sym[i]);
	  return sym[i];
	} 
  }
  return NULL;
}

/**
 * @brief Used to check if a string contains a reserved word 
 * We remove the first occurance of reserved word from string
 * Will only happen if the string starts with that word
 * returns the removed reserved word
 * 
 * @param str 
 * @return char* 
 */
char *check_res(char* str){
	for(int i = 0; i < nres; i++){
		if (fndstr(str, res[i]) == 0){
			// checking if we have symbol after 
			if(isalpha(str[strlen(res[i])]) || str[strlen(res[i])] == '_'){
				return NULL;
			}
			
			strrmv(str, res[i]);
			return res[i];
		} 
	}
	return NULL;
}

int check_str_space(){
	return 1;
}

/**
 * @brief Checks if we have integers 
 * Stops at first occurance of non integer 
 * Must start with an integer 
 * Removes that integer from the string 
 * Returns the integer
 * 
 * @param str 
 * @return char* 
 */
char* check_int(char *str)
{
  if(isdigit(str[0])){
	int j = 0;
	int i = 1;

	for(i = 1; i < strlen(str); i++){
	  if(!isdigit(str[i])){
		j = i;
		break;
	  }
	}
	if(i == strlen(str) && j == 0){
	  char *dst = malloc( (i) * sizeof(char) ); 
	  strcpy(dst, str);
	  dst[i] = 0;
	  strrmv(str, dst);
	  return dst;
	}
	char *dst = malloc( (j+1) * sizeof(char) ); 
	strncpy(dst, str, j);
	dst[j] = 0;
	strrmv(str, dst);
	return dst;
  }
  return NULL;
}

/**
 * @brief Only checks if a string begins with a reserved word 
 * Used for check_id  
 * This is used to prevent the mistake of considering 
 * reserved word as an ID 
 * 
 * @param str
 * @return int 
 */
int check_res_id(char *str){
	for(int i = 0; i < nres; i++){
		if (fndstr(str, res[i]) == 0){
			// checking if we have ID directly after it 
			if(isalpha(str[strlen(res[i])]) || str[strlen(res[i])] == '_'){
				return 1;
			}
			return 0;
		} 
	}
	return 1;
}

/**
 * @brief Checks if a string has an ID
 * Must start with that ID
 * removes the ID from original string and returns it 
 * 
 * @param str 
 * @return char* 
 */
char* check_id(char* str){
  if(check_res_id(str) && (isalpha(str[0]) || str[0] == '_')){
	int j = 0; 
	int i = 1;
	for(i = 1; i < strlen(str); i++){
	  if(!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_'){
		j = i;
		break;
	  }
	}
	if(i == strlen(str) && j == 0){
	  char *dst = malloc( (strlen(str)) * sizeof(char) ); 
	  strcpy(dst, str);
	  dst[i] = 0;
	  strrmv(str, dst);
	  return dst;
	}
	char *dst = malloc( (j+1) * sizeof(char) ); 
	strncpy(dst, str, j);
	dst[j] = 0;
	strrmv(str, dst);
	return dst;
  }
  return NULL;
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
  // vars 
  Token temp;
  tokens = (List*)malloc(sizeof(List));
  tokens->cl = 0;  
  tokens->size = 0;
  FILE *file = NULL;
  int line_number = 1;
  char *line = NULL;
  size_t len = 0;
  char *buff_res = NULL;
  char *buff_id = NULL;
  char *buff_int = NULL;
  char *buff_sym = NULL;
  char line_buff[1024];
  int line_number_str = 0;
  int line_number_ills = 0;
  int step = 0;
  State state = running_state;
  char word_with_space[1024];

  // opening file 
  file = fopen(file_name, "r"); // read mode

  // checkking if file is empty 
  if (file == NULL){
	printf("File not found \n");
	return 0;
  }

  // getting line by line 
  while(getline(&line, &len, file) != -1){
	
	// copying line to a buffer so we can use it later for strings
	strcpy(line_buff, line); 
	
	// extracting every string seperated by space
	char *word = strtok(line, " \t");

	// Looping through the words 
	while(word != NULL){

		strcpy(word_with_space, word);
	  // removing whitespace
	  rmvspc(word);


	  // checking the states 
	  if(state == hold_state || state == quit_eofstr 
	  || state == quit_nwlstr || state == quit_ills){
		word = strtok(NULL, " ");
		continue;
	  }

	  // check if its empty or not 
	  if(strcmp(word, "\0") != 0 && strlen(word) > 0){
		// check if state is running_state or comment_state 
		if(state != comment_state){
		  // COMMENTS
		  if(state != string_state){
			if(fndstr(word, "/**") == 0|| fndstr(word, "//") == 0 
			|| fndstr(word, "/*") == 0){
			  step = 1;
			  // state is comments if [/**] or [/*]
			  if(fndstr(word, "/**") == 0|| fndstr(word, "/*") == 0){
				state = comment_state;
				continue;
			  }
			  // go to next line if [//]
			  else{
				state = running_state;
				break;
			  }
			}
		  }

		  temp.ln = line_number;
		  strcpy(temp.fl, file_name);

		  // STRING 
		  if(word[0] == '"'){
			if(state == string_state){
			  chremv(word, '"');
			  state = running_state;
			}
			else{
			  if(modstr(line_buff, '"') == 2){
				// get indexes 
				int* indx = findch(line_buff, '"');

				// copying everything to the token 
				strslc(line_buff, temp.lx, indx[0]+1, indx[1]-1);

				temp.tp = STRING;
				push_back(&tokens, temp);

				line_number_str = 0;

				if(modstr(word, '"') == 2){
				  chremv(word, '"');
				  strers(word, '"');
				  state = running_state;
				  step = 0;
				}
				else{
				  chremv(word, '"');
				  state = string_state;
				  step = 1;
				}
			  }
			  else{
				line_number_str = line_number;
				state = hold_state;
				continue;
			  }
			}
			if(line_number_str != 0){
			  state == quit_nwlstr;
			  continue;
			}
		  }
		  if(state == string_state){
			if(line_number_str != 0 && line_number_str < line_number){
			  if(modstr(line_buff, '"') > 0){
				state = quit_nwlstr;
				break;
			  }else{
				state = quit_eofstr;
				break;
			  }
			}

			int *loc = findch(word, '"');
			if(loc == NULL){
			  strrmv(word, word);
			  step = 1;
			  state = string_state;
			  continue;
			}
			else{
			  strers(word, '"');
			  state = running_state;
			}
		  }

		  // RESWORD  
		  buff_res = check_res(word);
		  if(buff_res != NULL){
			temp.tp = RESWORD;
			strcpy(temp.lx, buff_res);
			push_back(&tokens, temp);
		  }

		  // SYMBOLS 
		  buff_sym = check_sym(word);
		  if(buff_sym != NULL){
			temp.tp = SYMBOL;
			strcpy(temp.lx, buff_sym);
			push_back(&tokens, temp);
		  }

		  // INT 
		  buff_int = check_int(word);
		  if(buff_int != NULL){
			temp.tp = INT;
			strcpy(temp.lx, buff_int);
			push_back(&tokens, temp);
		  }

		  // ID 
		  buff_id = check_id(word);
		  if(buff_id != NULL){
			temp.tp = ID;
			strcpy(temp.lx, buff_id);
			push_back(&tokens, temp);
		  }

		  if(strcmp(word, "") == 0){
			step = 1;
		  }

		  // Anything else 
		  if(strcmp(word, "") != 0 && 
		  buff_id == NULL && buff_int == NULL && 
		  buff_res == NULL && buff_sym == NULL && 
		  state != string_state){

			// undefined symbol 
			state = quit_ills;
			line_number_ills = line_number;
			step = 1;
		  }
		}    
		else{
		  step = 1;

		  // finish from multiline comments
		  if(strrmv(word, "*/")){
			state = running_state;
			// if we have [*//*] or [*//**]
			if(strrmv(word, "/**") 
			|| strrmv(word, "/*")){
			  state = comment_state;
			}
			// if we have [*///]
			else if (strrmv(word, "//")){
			  state = running_state;
			  break;
			}
		  }
		}
	  }
	  else{
		step = 1;
	  }
	  if(step){
		word = strtok(NULL, " "); // get the next token of the line 
		step = 0;
	  }
	}
	if(state == hold_state){
	  state = string_state;
	}
	else if(state == quit_eofstr || state == quit_nwlstr 
	|| state == quit_ills){
	  continue;
	}
	line_number++; // increase line number 
  }
  // checking if we reache end of file 
  if(feof(file)){
	// if we didnt end a comment 
	if(state == comment_state){
	  temp.tp = ERR;
	  temp.ln = line_number;
	  temp.ec = EofInCom;
	  strcpy(temp.lx, "Error: unexpected eof in comment");
	}
	// eof in string 
	else if(state == quit_nwlstr){
	  temp.tp = ERR;
	  temp.ln = line_number_str;
	  temp.ec = NewLnInStr;
	  strcpy(temp.lx, "Error: new line in string constant");
	}
	else if(state == quit_eofstr || state == string_state){
	  temp.tp = ERR;
	  temp.ln = line_number_str;
	  temp.ec = EofInStr;
	  strcpy(temp.lx, "Error: unexpected eof in string constant");
	}
	else if(state == quit_ills){
	  temp.tp = ERR;
	  temp.ln = line_number_ills;
	  temp.ec = IllSym;
	  strcpy(temp.lx, "Error: illegal symbol in source file");
	}
	// normal end of line token 
	else{
	  temp.tp = EOFile;
	  temp.ln = line_number+1;
	  strcpy(temp.lx, "End of File");
	}
	strcpy(temp.fl, file_name);
	push_back(&tokens, temp);
  }
  fclose(file);
  return 1;
}

// Get the next token from the source file
Token GetNextToken ()
{
	Token t;           
	t = tokens->value[tokens->cl];
	if(tokens->cl < tokens->size-1){
		tokens->cl++;
	}
	return t;
}

// peek (look) at the next token in the source file without
// removing it from the stream
Token PeekNextToken ()
{
	return tokens->value[tokens->cl];
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	free(tokens);
	return 0;
}



// //do not remove the next line
// #ifndef TEST
// int main ()
// {

// }
// // do not remove the next line
// #endif
