#include <stdlib.h>
#include "Types.h"

typedef struct {
    const char* stringType;
    Type type;
} TypeMap;

static const TypeMap typeMap[] = {
        {"int", TYPE_INT},
        {"bool", TYPE_BOOL},
        {"string", TYPE_STRING},
        {"void", TYPE_VOID},
        {NULL, TYPE_ERROR}  // ending
};


Type convertStringType(char* stringType)
{
    for (int i = 0; typeMap[i].stringType != NULL; i++) {
        if (strcmp(typeMap[i].stringType, stringType) == 0) {
            return typeMap[i].type;
        }
    }
    return TYPE_ERROR;
}

const char* convertTypeToString(Type type)
{
    for (int i = 0; typeMap[i].stringType != NULL; i++) {
        if (typeMap[i].type == type) {
            return typeMap[i].stringType;
        }
    }
    return NULL;
}