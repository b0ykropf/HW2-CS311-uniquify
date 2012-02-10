/* 
*    Author: Byron Kropf
*    Collaborators: Ian Fridge (gave assistence with parseing and suppressor)
*    Contact: kropfb@onid.orst.edu
*    Created: January 25 2012
*    Last Modified: February 2 2012
*
*    Explaination: Main file for the second assignment of CS311 which will
*    read a text file and output all of the unique words in the file, in 
*    alphabetical order.
*    Example call:
*/

#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "error.h"
#include "bunique_functions.h"

#define WORD_SIZE 100
#define MAX_TOKEN 2000
#define FALSE 0
#define TRUE !FALSE




int main(int argc,char *argv[]){

    if( argc != 2){ //Check to see if enough arguments were given
        err_exit("ERROR: You did not give 1 arguments.");
    }

    int kid_count = atoi(argv[1]); //stores first argument as number of children to create

    char* token = NULL;
    int traffic_cop = 0;
    int length;
    int status;

    pid_t* sort_id = NULL;
    pid_t  supp_id;

    int** pipe_farm_ps = NULL; // fds for pipes for parent to sorts
    int** pipe_farm_sr = NULL; // fds for pipes for sorts to suppressor

    FILE** pp_ps = NULL; //pipe pointers for parent to sorts
    FILE** pp_sr = NULL; //pipe pointers for sorts to suppressor
    
    //Lock memory for pipes, pointers, and pids.
    sort_id = (pid_t*) malloc(sizeof(pid_t) * kid_count);
    pipe_farm_ps = (int**) malloc(sizeof(int*) * kid_count);
    pipe_farm_sr = (int**) malloc(sizeof(int*) * kid_count);
    pp_ps = (FILE**) malloc(sizeof(FILE*) * kid_count);
    pp_sr = (FILE**) malloc(sizeof(FILE*) * kid_count);

    token = (char*) malloc(sizeof(char) * MAX_TOKEN);



    for(int i = 0; i < kid_count; i++){
        
        pipe_farm_ps[i] =  malloc(sizeof(int) * 2);
        pipe_farm_sr[i] =  malloc(sizeof(int) * 2);

        if( pipe(pipe_farm_ps[i]) == -1 ) { //create pipes in pipe_farm_ps
            err_exit("ERROR: Problem creating pipe ps");
        }

        if((pp_ps[i] = fdopen(pipe_farm_ps[i][1],"w")) == NULL){
            err_exit("ERROR: Problem making pointer to pfdps.");
        }

        if( pipe(pipe_farm_sr[i]) == -1 ) { //create pipes in pipe_farm_sr
            err_exit("ERROR: Problem creating pipe sr");
        }

        if((pp_sr[i] = fdopen(pipe_farm_ps[i][1],"w")) == NULL){
            err_exit("ERROR: Problem making pointer to pfdsr.");
        }
    }
    
    for(int i = 0; i < kid_count; i++){ //Sorting kids
        if((sort_id[i] = fork()) == 0){

            for(int k = 0; k < kid_count; k++){
                if(i == k){
                    if(close(pipe_farm_ps[i][1]) != 0){
                        err_exit("ERROR: closing write end of pipe in child\n");
                    }
                    if(close(pipe_farm_sr[i][0]) != 0){
                        err_exit("ERROR: closing read end of pipe in child\n");
                    }
                    dup2(pipe_farm_ps[i][0], 0);
                    dup2(pipe_farm_sr[i][1], 1);
                }
                else{
                    if(close(pipe_farm_ps[k][1]) != 0){
                        err_exit("ERROR: closing write end of pipe in child\n");
                    }
                    if(close(pipe_farm_ps[k][0]) != 0){
                        err_exit("ERROR: closing read end of pipe in child\n");
                    }
                    if(close(pipe_farm_sr[k][1]) != 0){
                        err_exit("ERROR: closing write end of pipe in child\n");
                    }
                    if(close(pipe_farm_sr[k][0]) != 0){
                        err_exit("ERROR: closing read end of pipe in child\n");
                    }
                }
            }
            execl("/usr/bin/sort","sort",NULL); // make child into sort();
            _exit(1);
        }
    } // END Sorting kids
fflush(stdout);
    if((supp_id = fork()) == 0){ //Suppresor kid
        int sorters = kid_count;
        int* valid_sources = NULL;
        int token_count = 0;
        char* last_word = NULL;
        char** words = NULL;

        valid_sources = init_reader_table(sorters);
        words = (char**) malloc(sizeof(char*) * MAX_TOKEN);
        last_word = (char*) malloc(sizeof(char) * WORD_SIZE);

printf("LOLZ\n");
printf("%d%s\n",valid_sources[2],"Suppressor Check 1: \n");
        for(int i = 0; i < kid_count; i++){
            if(close(pipe_farm_ps[i][1]) != 0){
                err_exit("ERROR: closing write end of pipe in child\n");
            } 
            if(close(pipe_farm_ps[i][0]) != 0){
                err_exit("ERROR: closing read end of pipe in child\n");
            }
            if(close(pipe_farm_sr[i][1]) != 0){
                err_exit("ERROR: closing write end of pipe in child\n");
            }
            if((pp_sr[i] = fdopen(pipe_farm_sr[i][0], "r")) == NULL){
                err_exit("ERROR:creating file pointer from file descriptor suppressor\n");
            }
        }
printf("%s \n", "Suppressor Check 2: ");
        while(sorters > 0){
printf("%s \n", "Suppressor Check 3a: ");
            for(int i = 0; i < sorters; i++){
               traffic_cop = valid_sources[i];
printf("%d",traffic_cop);
printf("%s \n", "Suppressor Check 3b: ");

               if(fgets(token, WORD_SIZE , pp_sr[traffic_cop]) != NULL){
                   printf("%s \n", token);
                     if((length = strlen(token)) <= 2){
                     printf("%s \n","I AM HERE 1");
                        continue;
                    }
printf("%s \n", "Suppressor Check 3c: ");
                    if(token_count < MAX_TOKEN){
                        words[token_count] = (char*) malloc(sizeof(char) * (length + 1 ));
                        strcpy(words[token_count],token);
                        token_count++;
                        insert_sort(words, token_count);
                }
                    else{
                        token_count--;
                        if(strcmp(last_word, words[token_count]) != 0){
                            strcpy(last_word, words[token_count]);
                            printf("%s", words[token_count]);
                        }
                        token_count = remove_duplicate(words, token_count, words[token_count]);
                        length = strlen(words[token_count]);
                        strcpy(words[token_count], token);
                        token_count++;
                        insert_sort(words, token_count);
                    }
                }
                else{
                    printf("Kid dying: %d\n",traffic_cop);
                    sorters--;
                    remove_sorter(valid_sources,sorters,traffic_cop);
                }
            }
        }
printf("%s \n", "Suppressor Check 4: ");
        free(words);
        words = NULL;
        for(int i = 0; i < kid_count; i++){
            if(close(pipe_farm_sr[i][0]) != 0){
                err_exit("ERROR: closing pipe in suppressor");
            }
            free(pipe_farm_sr[i]);
            pipe_farm_sr[i] = NULL;
            free(pipe_farm_ps[i]);
            pipe_farm_ps[i] = NULL;
        }
        printf("%d",token_count);
        free(token);
        token = NULL;
        free(last_word);
        last_word = NULL;
        free(pp_sr);
        pp_sr = NULL;
        free(pp_ps);
        pp_ps = NULL;
        printf("Close Kid\n");
        _exit(EXIT_SUCCESS);
   } //END Supressor kid
fflush(stdout);

//Finish parent instructions
    while(fscanf(stdin,"%s",token) != EOF) {
        length = strlen(token);
        add_cr(token,length);
        lower_case(token, length);
        fputs(token, pp_ps[traffic_cop]);
printf("%s\n",token);
        if(traffic_cop == (kid_count - 1)){ // if we used last pipe, wrap around
            traffic_cop = -1;
        }
        traffic_cop++;
printf("%d",traffic_cop);
    } 
 
fflush(stdout);
    for(int i = 0; i < kid_count; i++){
        if(close(pipe_farm_ps[i][1]) != 0){
            err_exit("Error closing write end of pipe in parent\n");
        }
        if(close(pipe_farm_ps[i][0]) != 0){
            err_exit("Error closing read end of pipe in parent\n");
        }
        free(pipe_farm_ps[i]);
        free(pipe_farm_sr[i]);
    } 

    while (waitpid(-1,&status,WNOHANG) > 0){
        printf("Child exited");
    }
    free(token);
    token = NULL;
    free(pp_ps);
    pp_ps = NULL;
    free(pipe_farm_ps);
    free(pipe_farm_sr); 
    exit(EXIT_SUCCESS); 
}// END main
