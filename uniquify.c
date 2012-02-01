/* 
*    Author: Byron Kropf
*    Contact: kropfb@onid.orst.edu
*    Created: January 25 2012
*    Last Modified: January 25 2012
*
*    Explaination: Main file for the second assignment of CS311 which will
*    read a text file and output all of the unique words in the file, in 
*    alphabetical order.
*    Example call:
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "error.h"

#define FALSE 0
#define TRUE !FALSE

struct kid_stats{
    int pid;
    int ppipe[2];
    FILE* file_out;
    struct kid_stats* next;
};

typedef struct kid_stats kid;

int main(int argc,char *argv[]){

    if( argc != 2){ //Check to see if enough arguments were given
        err_exit("ERROR: You did not give 2 arguments.");
    }

    int kid_count = atoi(argv[1]); //stores first argument as number of children to create
    pid_t kid_id;
    
    char *in_path;
    char buffer[8];

    int** pipe_farm = NULL;
    pipe_farm = (int**) malloc( sizeof(int*) * kid_count); //mallocs memory for pointers

    FILE** tmp_files = NULL;
    tmp_files = (FILE**) malloc( sizeof(FILE*) * kid_count);

    char* test = "This is a test.";

    for( int i = 0; i < kid_count; i++ ){
        pipe_farm[i] = malloc( sizeof(int) * 2 );//malloc memory for pipes
        if( pipe(pipe_farm[i]) == -1 ) { //create pipes in pipe_farm
            err_exit("ERROR: Problem creating pipe");
        }
        tmp_files[i] = malloc( sizeof(FILE*) );//malloc memory for temp files
        tmp_files[i] = tmpfile(); //create temp files

    }
    
    for( int i =0; i < kid_count; i++ ) {
        switch( ( kid_id = fork() ) ) { //Spawn Children (Pray that they do not zombify)

            case 0:
                
                if( close(pipe_farm[i][1]) != 0 ) {
                    err_exit("ERROR:Problem plugging write end of pipe to child ",i);
                }
                
                if( i != 0 ){
                    for( int k = 0; i < k; k++ ) { //For each child close uneeded pipes
                        close(*pipe_farm[k]);
                    }
                }
           
                dup2(fileno(tmp_files[i]), 1);
                dup2(pipe_farm[i][0], 0);
                snprintf(argv[1], 1, "%d", i);
                execve("./kid", argv, NULL);
                _exit(1);

            default:
                
                for(int i = 0; i < kid_count; i++) {
                    if( close(pipe_farm[i][0]) ) {
                        err_exit("ERROR: Problem closeing read end of pipes for parent");
                    }
                }
                for(i = 0; i < kid_count; i++) {
                    
                    write(pipe_farm[i][1], test, buffer); //write test to each pipe
                    close(pipe_farm[i][1]); //Plug the pipes

                }

                wait(NULL); //Wait for kids (DO NOT LET THEM ZOMBIFY!)
                for(i = 0; i < kid_count; i++) {
                    fseek(tmp_files[i], 0, SEEK_SET);
                    fgets(buffer, 8, tmp_files[i]);
                    printf("%s\n", buffer);
                }
                exit(1);
        }
    }
    
 }
