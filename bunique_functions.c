#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void add_cr(char* input, int length) { //function to add newline after each word
    for(int i = 0; i < length; i++){
        if(isalpha(input[i]) == 0){
            strcpy(input + i, "\n");
            return;
        }
    }
    strcpy(input + length, "\n\0");
}

void lower_case(char* input, int length) { //function to convert string to lower case
    for(int i = 0; i < length; i++) {
        input[i] = tolower(input[i]);
    }
}

void insert_sort(char** words, int length) {//function to sort back into suppressor
    char* current;
    int i;
    int j = 0;
    for(i = 1; i < length; i++) {
        current = words[i];
        for(j = i - 1; j >= 0 && strcmp(words[j], current) < 0; j--) {
            words[j + 1] = words[j];
        }
        words[j + 1] = current;
    }
}

int remove_sorter(int* sorters, int total, int sorter){ //function to remove empty sorters
    for(int i = 0; i < total; i++){
        if(sorters[i] == sorter){
            for(int k = i + 1; i < total; i++){
                sorters[i] = sorters[k];
                ++i;
            }
            return total - 1;
        }
    }
    return -1;
}

int remove_duplicate(char** words, int tokens, char* to_remove){//function to suppress
    int i;
    for(i = tokens; 0 < i; i--){
        if(strcmp(to_remove, words[i]) == 0){
            return i;
        }
    }
    return tokens - 1;
}

int* init_reader_table(int readers) {
int* table = NULL;
table = (int*) malloc(sizeof(int) * readers);
for(int i = 0; i < readers; i++){
table[i] = i;
}
return table;
}

