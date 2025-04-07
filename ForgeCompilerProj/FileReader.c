#pragma warning (disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024  

/// <summary>
/// This func reads the contents of a given text file
/// </summary>
/// <param name="filename"></param>
/// <returns>Returns the contentes of the file</returns>
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rt");  
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }
    int bufferSize = 4096;  
    char* buffer = (char*)malloc(bufferSize);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    buffer[0] = '\0';  
    char line[MAX_LINE_LENGTH];  
    int firstLine = 1;  

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // resize buffer if needed
        if (strlen(buffer) + strlen(line) + 1 > bufferSize) {
            bufferSize *= 2;  
            char* temp = (char*)realloc(buffer, bufferSize);
            if (!temp) 
            {
                fprintf(stderr, "Error: Unable to realloc buffer in file read\n");
                fclose(file);
                return NULL;
            }
            buffer = temp;
        }
        // concat the line to the buffer
        strcat(buffer, line);  
    }
    fclose(file);
    return buffer;
}


/// <summary>
/// This func is used to trim the starting and ending whitespaces in a line
/// </summary>
/// <param name="str"></param>
/// <returns>Returns the data with no starting or ending spaces</returns>
char* trim(char* str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}