#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//get_user - used from deviewer, modified to remove character spaces

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

char* getUser(void){
    system("who > user.txt");

    FILE* fs;

    char buf[10] = {};
    char* results;

    fs = fopen("user.txt", "r");

    fgets(buf, 10, fs);
    results = buf;

    remove("user.txt");

    remove_spaces(results);
    
    return results;
}

