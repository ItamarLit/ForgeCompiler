#pragma warning(disable:4996)
#include "FileReader.h"

char* readFile(const char* filename) {
    // open the file
    FILE* file = fopen(filename, "rb");
    char* buffer;
    long fileSize;
    // check if the file was opened
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }
    // file pointer to the end to determine the file size
    if (fseek(file, 0, SEEK_END) != 0) {
        printf("Error: fseek failed for file %s\n", filename);
        fclose(file);
        return NULL;
    }
    // get the file size
    fileSize = ftell(file);
    if (fileSize == -1L) {
        printf("Error: ftell failed for file %s\n", filename);
        fclose(file);
        return NULL;
    }
    // reset the file pointer to the beginning of the file
    if (fseek(file, 0, SEEK_SET) != 0) {
        printf("Error: fseek failed to rewind file %s\n", filename);
        fclose(file);
        return NULL;
    }
    // memory for the file contents 
    buffer = (char*)malloc((fileSize + 1) * sizeof(char));
    if (buffer == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    // read file into the buffer
    unsigned int bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead != fileSize) {
        printf("Error: fread failed. Expected %ld bytes, got %zu bytes\n", fileSize, bytesRead);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[fileSize] = '\0';
    // close the file
    fclose(file);
    return buffer;
}


