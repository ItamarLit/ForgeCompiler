#ifndef FILE_READER_H
#define FILE_READER_H


#include <stdio.h>
#include <stdlib.h>

// func that will return a string of the file content
char* read_file(const char* filename);
// trim func that removes whitespaces before and after the data
char* trim(char* str);

#endif

