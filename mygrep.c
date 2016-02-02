/* assigment 1 - mygrep.c final submission 
 * DESC: sequential search and pattern matching program 
 * NAME: 
 * ID  : 
 * DATE: 20/09/2015
 * NOTE: 01111100
 *       algorithms are fun!
 */

//--------// preprocessors //-------------------------------------//

/* included standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* general constants */
#define EXPECTED_ARGC     2     /* nums of arguments this program takes */
#define MAXCHAR_QUERY     100   /* nums of chars a query can have       */
#define MAXCHAR_LINE      1000  /* nums of chars a line can have        */
#define MAXLINE_RETAINED  10    /* nums of lines allowed to be retained */

/* control constants */
#define DEBUG             0     /* controls the debug output            */
#define DEBUG_MATRIX      0     /* controls the score matrix output     */

/* functions constats */
#define ONE               1     /* PrintStage() - specific constants    */
#define TWO               2     /* used to indicate which output this   */
#define THREE             3     /* function should print                */
#define FOUR              4     /*                                      */

#define UNINITIALIZED    -1     /* line_t - related constants           */
                                /* used to indicate that the value of   */
                                /* the variable is uninitialized        */

//--------// struct definitions //--------------------------------//

/* line_t - represents a line information
 *
 * index: nth line from stdin
 * chars: a char pointer that holds the string this line represents
 *  size: bytes of characters the line contains
 * score: holds the computed score for this line
 */

typedef struct LINES {
	int index; 
	char *chars;
	size_t size;
	double score;
} line_t;

/* dict_t - holds line pointer and nums of populated element
 *
 *         line: a line_t pointer that holds the top 10 scoring lines
 * member_count: number of (populated) members in this LINE_DICT
 */

typedef struct LINE_DICTS {
	line_t *line;
	int member_count;
} dict_t;

//--------// function prototypes //-------------------------------//

line_t CreateLine(int line_index, char* string);
char *CleanString(char *string);  
void PrintStage(int stage, line_t line_in);
void PrintDebug(char* string);
double GetSimpleScore(line_t line_in, char *query);
double GetComplexScore(line_t line_in, char *raw_qery);
double GetRowScore(char *string, char *i_query, int i);
char *GetReduceString(char *string);
char *GetLowercase(line_t line_in);
dict_t CreateNewDict(int capacity);
void RefreshDict(dict_t *line_dict, line_t line_in);

//--------// main() function //-----------------------------------//

int main(int argc, char* argv[]){
	/* argument count check */
	if(argc != EXPECTED_ARGC){
		printf("Please input the appropriate argument(s)\n");
		exit(EXIT_FAILURE);
	}

	/* stage 0 - get query */
	char *query = argv[1];
	printf("S0: query = %s \n", query);

	int line_count = 0;
	char retrieved_string[MAXCHAR_LINE];
	line_t current_line;

	/* create line_dict to store top scoring lines */
	dict_t line_dict = CreateNewDict(MAXLINE_RETAINED);

	while(fgets(retrieved_string, MAXCHAR_LINE, stdin) != NULL){
		line_count++;
		
		/* skip empty lines containing only non-printable chars */
		if(strlen(retrieved_string) == 1 && !isprint(*retrieved_string)){
			continue;
		}

		{ /* DEBUG INFO */
			#if DEBUG
			printf("retrieved_string:\n");
			PrintDebug(retrieved_string);
			#endif
		}

		/* stage 1 - get lines */
		current_line = CreateLine(line_count, retrieved_string);
		printf("---\n");
		printf("%s\n", current_line.chars);
		PrintStage(ONE, current_line);

		/* stage 2 - simple score */
		current_line.score = GetSimpleScore(current_line, query);
		PrintStage(TWO, current_line);

		/* stage 4 - complex score */
		current_line.score = GetComplexScore(current_line, query);
		PrintStage(FOUR, current_line);

		/* retain top scoring lines */
		if(current_line.score != 0){
			RefreshDict(&line_dict, current_line);
		}

		else {
			/* free pointer from CleanString() */
			free(current_line.chars);
			current_line.chars = NULL;
		}
	}

	/* stage 3 - ranked output */
	printf("------------------------------------------------\n");
	int i = 0;
	while(i < line_dict.member_count){
		PrintStage(THREE, line_dict.line[i]);
		printf("%s\n", line_dict.line[i].chars);
		printf("---\n");

		/* free pointer from CleanString() */
		free(line_dict.line[i].chars);
		line_dict.line[i].chars = NULL;
		i++;
	}
	/* free pointer from CreateNewDict() */
	free(line_dict.line);
	line_dict.line = NULL;


	return 0;
}

//--------// CreateLine() //--------------------------------------//

/* DESC: creates a line according to a the information
 *       given by the parameter
 */

line_t CreateLine(int line_index, char* string){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("CreateLine() <- string\n");
		PrintDebug(string);
		#endif
	}
	line_t new_line;
	new_line.index = line_index;
	new_line.chars = CleanString(string);
	new_line.size = sizeof(char) * strlen(new_line.chars);
	new_line.score = UNINITIALIZED;
	return new_line;
}

//--------// CleanString() //-------------------------------------//

/* DESC: takes in a string and returns a new '\0'-terminated
 *       char pointer containing only printable chars from string
 * NOTE: this function is created in order to substitute strcpy()
 *       and/or strncpy() with a more reliable function
 * NOTE: malloc() is used here
 */

char *CleanString(char *string){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("CleanString() <- string\n");
		PrintDebug(string);
		#endif
	}
	/* counts printable chars in string */
	int char_count = 0;
	while(isprint(string[char_count])){
		char_count++;
	}

	/* allocate enough memory for new_string */
	size_t bytes = char_count * sizeof(char);
	char *new_string = (char*) malloc(bytes + sizeof(char));
	
	/* malloc() fail check  */
	if (new_string == NULL){
		printf("CleanString() ERROR: MEMORY ALLOCATION FAIL\n");
		exit(EXIT_FAILURE);
	}

	/* assigning characters to new pointer and '\0'-terminate */
	memcpy(new_string, string, bytes);
	new_string[char_count] = '\0';
	return new_string;
}

//--------// PrintStage() //-------------------------------------//

/* DESC: prints out relevant line information according to stage */

void PrintStage(int stage, line_t line_in){
	/* stage parameter check */
	if(stage < ONE || stage > FOUR){
		printf("PrintStage() ERROR: INVALID STAGE\n");
		exit(EXIT_FAILURE);
	}

	/* print information */
	printf("S%d: line%6d, ", stage, line_in.index);
	if(stage == ONE){
		printf("bytes =%3ld\n", (unsigned long)line_in.size);
	}
	else {
		printf("score =%7.3f\n", line_in.score);
	}	
	return;
}


//--------// PrintDebug() //--------------------------------------//

/* DESC: prints some of the common non-printing chars for debugging */
void PrintDebug(char* string){
	char c;
	int i = 0;
	while(1){
		c = string[i];
		if(c == '\0'){
			printf("%c%c\n", 92, 48);
			break;
		}
		else if(c == '\n'){
			printf("%c%c", 92, 110);
		}
		else if(c == '\r'){
			printf("%c%c", 92, 114);
		}
		else if(c == '	'){
			printf("TAB");
		}
		else if(c == ' '){
			printf("_");
		}
		else{
			printf("%C", c);
		}
		i++;
	}
	return;
}

//--------// GetSimpleScore() //------------// stage 2 //--------//

/* DESC: evaluates simple score for line */

double GetSimpleScore(line_t line_in, char *query){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("GetSimpleScore() <- query\n");
		PrintDebug(query);
		#endif
	}

	double score = 0;
	int match_count = 0;
	int i = 0, j = 0;
	char *start_ptr;

	while(query[i] != '\0'){
		/* iterate through query and look for
		 * the first matching char from string
		 */

		start_ptr = strchr(line_in.chars, query[i]);

		/* only execute loop if matching char is found */
		while(start_ptr != NULL){
			j = 0;
			
			/* count consecutive matching char */
			while(start_ptr[j] == query[i+j]){
				match_count++;
				j++;
				
				/* pointer boundary check */
				if(start_ptr[j] =='\0' || query[i+j] == '\0'){
					break;
				}
			}

			/* update score if needed and reset match_count */
			if(match_count > score){
				score = match_count;
			}
			match_count = 0;

			/* pointer boundary check for last element
			 * (when start_ptr gets incremented)
			 * and look for next matching char from
			 * the rest of the string
			 */

			start_ptr++;
			if(*start_ptr == '\0'){
				break;
			}
			else {
				start_ptr = strchr(start_ptr, query[i]);
			}
		}
		i++;
	}
	return score;
}

//--------// GetComplexScore() //------------// stage 4 //--------//

/* DESC: takes in line_in and compares it against the query,
 *       evaluating the score according to the conditions
 *       and formula written on the assignment1 specification
 */

double GetComplexScore(line_t line_in, char *raw_query){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("GetComplexScore() <- raw_query\n");
		PrintDebug(raw_query);
		#endif
	}

	{ /* DEBUG SCORE MATRIX */
		#if DEBUG_MATRIX
		int matrix_index = 0;
		#endif
	}
	/* condition 1 - query pattern is reduced */
	char *query = GetReduceString(raw_query);

	/* condition 2 - comparison made are case-insensitive */
	char *string = GetLowercase(line_in);

	double score = 0;
	int i = 0;
	while(query[i] != '\0'){
		{ /* DEBUG SCORE MATRIX */
			#if DEBUG_MATRIX
			printf("DB: ");
			if (i == 0){
				matrix_index = 0;
				while(matrix_index < strlen(query)){
					printf("  %c", query[matrix_index]);
					matrix_index++;
				}
				printf("\nDB: ");
			}
			matrix_index = 0;
			while(matrix_index < i){
				//printf("  0"); /* alteration from specification */
				printf("  %c", 35);
				matrix_index++;
			}
			#endif
		}
		/* calculate the sum of a row matrix */
		score = score + GetRowScore(string, &query[i], i);
		i++;
	}

	/* calculate final score */
	score = score / (
		  log(30 + (int) line_in.size) / log(2)
		);

	/* free pointer from GetReducedString() */
	free(query);
	query = NULL;
	
	/* free pointer from GetLowerCase() */
	free(string);
	string = NULL;
	
	return(score);
}

//--------// GetRowScore() //----------------// stage 4 //--------//

/* DESC: evaluate a row of scoring matrix by comparing it to 
 *       partial query (i_query) and calculates
 *       the summation part (sigma) of the scoring regime 
 *       from assignment1 specification.
 */

double GetRowScore(char *string, char *i_query, int i){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("GetRowScore() <- string\n");
		PrintDebug(string);
		printf("GetRowScore() <- i_query\n");
		PrintDebug(i_query);
		#endif
	}

	{ /* DEBUG SCORE MATRIX */
		#if DEBUG_MATRIX
		int matrix_index = 0;
		#endif
	}

	/* create an array to represent each char in
	 * the query and initialize them to 0
	 */

	int i_matrix[strlen(i_query)];
	int index = 0;
	while(index < strlen(i_query)){
		i_matrix[index] = 0;
		index++;	
	}

	/* look for the first matching char from string */
	char* start_ptr = strchr(string, *i_query);
	
	int j = 0;
	/* only execute loop if matching char is found */
	while(start_ptr != NULL){

		/* update i_matrix for every consecutive match */
		while(start_ptr[j] == i_query[j]){
			i_matrix[j]++;
			j++;

			/* pointer boundary checking */
			if(start_ptr[j] =='\0' || i_query[j] == '\0'){
				break;
			}
		}

		/* pointer boundary check for last element	
		 * (when start_ptr gets incremented)
		 * and look for next matching char from
		 * the rest of the string
		 */

		j = 0;
		start_ptr++;
		if (*start_ptr == '\0'){
			break;
		}
		else {
			start_ptr = strchr(start_ptr, *i_query);
		}
	}

	{ /* DEBUG SCORE MATRIX */
		#if DEBUG_MATRIX
		while(matrix_index < strlen(i_query)){
			printf("%3d", i_matrix[matrix_index]);
			matrix_index++;
		}
		printf("\n");
		#endif
	}

	/* calculate score according to assignment1 specification */
	j = 2;
	double temp = 0;
	double row_score = 0;
	while(i_query[j] != '\0' && (strlen(i_query) > 2)){
		/* condition 3 - zero occurrence excluded */
		if(i_matrix[j] != 0){
			temp = (j - 1) * (j - 1);
			temp = temp * (
				  (log(2 + i_matrix[j])) / log(2)
				);
			row_score = row_score + temp;
			temp = 0;
		}
		j++;
	}
	return row_score;
}

//--------// GetReduceString() //------------// stage 4 //--------//

/* DESC: takes in string and returns new '\0'-terminated
 *       pointer that contains only alphanumeric chars
 *       in lower case for case-insensitive comparison 
 * NOTE: malloc() is used here
 */

char *GetReduceString(char *string){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("GetReduceString() <- string\n");
		PrintDebug(string);
		#endif
	}
	/* count the number of chars to be stored */
	int char_count = 0, i = 0;
	while(string[i] != '\0'){
		if(isalnum(string[i])){
			char_count++;
		}
		i++;
	}

	/* allocate enough memory for new_string */
	size_t bytes = char_count * sizeof(char);
	char *new_string = (char*) malloc(bytes + sizeof(char));

	/* malloc() fail check  */
	if (new_string == NULL){
		printf("GetReduceString() ERROR: MEMORY ALLOCATION FAIL\n");
		exit(EXIT_FAILURE);
	}

	/* assign alphanumeric chars from string to
	 * new_string as lowercase letters, and '\0'-terminate
	 */

	i = 0;
	while(*string != '\0'){
		if(isalnum(*string)){
			new_string[i] = tolower(*string);
			i++;
		}
		string++;
	}
	new_string[i] = '\0';
	return new_string;
}

//--------// GetLowercase() //---------------// stage 4 //--------//

/* DESC: takes in line_in and returns a new '\0'-terminated 
 *       pointer that contains lowercase string from line_in
 * NOTE: malloc() is used here
 */

char *GetLowercase(line_t line_in){
	{ /* DEBUG INFO */
		#if DEBUG
		printf("GetLowercase() <- line_in.chars\n");
		PrintDebug(line_in.chars);
		#endif
	}

	/* allocate enough memory for new_string and assign chars
	 * from line_in.chars as lowercase letters if possible
	 * and '\0'-terminate
	 */
	
	char *new_string = (char*) malloc(line_in.size + sizeof(char));
	
	/* malloc() fail check  */
	if (new_string == NULL){
		printf("GetLowercase() ERROR: MEMORY ALLOCATION FAIL\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	while(i < line_in.size){
		new_string[i] = tolower(line_in.chars[i]);
		i++;
	}
	new_string[i] = '\0';
	return new_string;
}

//--------// CreateNewDict() //--------------// stage 3 //--------//

/* DESC: creates line_dict of specified capacity
 * NOTE: malloc() is used here
 */

dict_t CreateNewDict(int capacity){
	size_t bytes = capacity * sizeof(line_t);
	line_t *line_ptr = (line_t*) malloc(bytes);

	/* malloc() fail check  */
	if (line_ptr == NULL){
		printf("CreateNewDict() ERROR: MEMORY ALLOCATION FAIL\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	/* set values to uninitialized to avoid unexpected behaviors */
	while(i < capacity){
		line_ptr[i].index = UNINITIALIZED;
		line_ptr[i].chars = NULL;
		line_ptr[i].size  = UNINITIALIZED;
		line_ptr[i].score = UNINITIALIZED;
		i++;
	}
	/* assign pointer to new_dict and set populated element as empty */
	dict_t new_dict = {line_ptr, 0};
	return new_dict;
}

//--------// RefreshDict() //----------------// stage 3 //--------//

/* DESC: takes line_in and add-sort items in line_dict
 *       line_dict holds the top 10 scoring lines
 */

void RefreshDict(dict_t *line_dict, line_t line_in){
 	line_t held_line = line_in; /* represent the line being assessed */
	line_t temp_line;
 	{ /* DEBUG INFO */
		#if DEBUG
		printf("holding line %04d (index number)\n", held_line.index);
		#endif
	}

 	/* check if current line has score larger than any line_dict members */
 	int i = 0;
	while(i < line_dict->member_count){
		if(held_line.score > line_dict->line[i].score){
			/* swap the two lines with the help of temp_line */
			temp_line = line_dict->line[i];
			line_dict->line[i] = held_line;
			held_line = temp_line;
		

			{ /* DEBUG INFO */
				#if DEBUG
				printf("swapped line %04d",line_dict->line[i].index);
				printf("with line %04d\n", held_line.index);
				printf("now holding line %04d\n", held_line.index);
				#endif
			}
		}
		i++;
	}
	
	/* add held_line into line_dict if there are unpopulated elements */
	if(i < MAXLINE_RETAINED){
		line_dict->line[i] = held_line;
		line_dict->member_count++;
		
		{ /* DEBUG INFO */
			#if DEBUG
			printf("added line %04d to line_dict\n", held_line.index);
			printf("line_dict has %3d members\n", line_dict->member_count);
			#endif
		}
	}
	/* held_line has to be disposed, free pointer from CleanString() */
	else {
		{ /* DEBUG INFO */
			#if DEBUG
			printf("line %04d has been incinerated", held_line.index);
			#endif
		}

		free(held_line.chars);
		held_line.chars = NULL;
	}
	return;
}

//----------------------------------------------------------------//

/* reference list:
 * http://blog.liw.fi/posts/strncpy/
 * http://www.tutorialspoint.com/cprogramming
 * 
 * commenting style adapted from:
 * 2013 Assignment Sample Solution - by Alistair Moffat
 */

//----------------------------------------------------------------//