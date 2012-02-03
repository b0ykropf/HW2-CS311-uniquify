/* 
*    Author: Byron Kropf
*    Contact: kropfb@onid.orst.edu
*    Created: January 25 2012
*    Last Modified: February 1 2012
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
    int ipipe[2];
    FILE* file_out;
    struct kid_stats* next;
};

typedef struct kid_stats kid;

int main(int argc,char *argv[]){

    if( argc != 2){ //Check to see if enough arguments were given
        err_exit("ERROR: You did not give 1 arguments.");
    }

    int kid_count = atoi(argv[1]); //stores first argument as number of children to create
    pid_t kid_id;
    pid_t kid_list[kid_count];// An array to store the pids of the kids. Not necessary.;

    int** pipe_farm = NULL;
    pipe_farm = (int**) malloc( sizeof(int*) * kid_count * 2); // mem for pipe pointers

    FILE** tmp_files = NULL;
    tmp_files = (FILE**) malloc( sizeof(FILE*) * kid_count); // mem for tmp pointers

    char* test = "This is a test.";

    size_t buffer = 8;

    for( int i = 0; i < (2 * kid_count); i++ ){ //makes 2 pipes per child
        pipe_farm[i] = malloc( sizeof(int) * 2 );//malloc memory for pipes
             
  
        if( pipe(pipe_farm[i]) == -1 ) { //create pipes in pipe_farm
            err_exit("ERROR: Problem creating pipe");
        }
        
    //Note: pipes 0-n are for parse to sort, and n+1-2n are for sort to suppress
    }
    
    for ( int i = 0; i < kid_count; i++){ // Create a temp file for each sorting child
        tmp_files[i] = malloc( sizeof(FILE*) );
        tmp_files[i] = tmpfile();
    }
 
    for( int i = 0; i <= kid_count; i++ ) {//spawns argv[1]+1 kids, +1 is suppressor
       
        switch( ( kid_id = fork() ) ) { //Spawn Children (Pray that they do not zombify)

            case 0:

                if(i == kid_count){ //Special instructions for suppressor
                    for( int j = 0; j < kid_count; j++){ //plug uneeded pipes
                        close(*pipe_farm[j]);
                    }

                    for( j = kid_count; j < (2 * kid_count); j++){ //plug write ends
                        if( close(pipe_farm[j][1]) != 0) {
                            err_exit("ERROR:Problem plugging write end pipe to suppres");
                        }
                    }
                }//End special instructions for suppressor

                if(i != kid_count){
                    if( close(pipe_farm[i][1]) != 0 ) {
                        err_exit("ERROR:Problem plugging write end of pipe to child ",i);
                    }

                    if( close(pipe_farm[kid_count + i][0]) != 0) { //plug read to suppres
                        err_exit("ERROR:Problem plugging read end of pipe to suppressor");
                    }

                    for( int k = 0;k < kid_count;k++ ) { //children plug uneeded in pipes
                        if(k != i){
                            close(*pipe_farm[k]);
                        }
                    } 
           
                   for( k = kid_count;k < (2 * kid_count); k++) { //also close uneeded out
                       if(k != (kid_count + i){
                           close(*pipe_farm[k]);
                       }
                   }
                }

                dup2(fileno(tmp_files[i]), 1);
                dup2(pipe_farm[i][0], 0);
                snprintf(argv[1], 1, "%d", i);
                execve("./kid", argv, NULL);
                _exit(1);

            default:
                
                for(int i = 0; i < (2 * kid_count); i++) { //Parent never reads from pipe
                    if( close(pipe_farm[i][0]) ) {
                        err_exit("ERROR: Problem closeing read end of pipes for parent");
                    }
                }

                for(i = 0; i < kid_count; i++) { //write to sort pipes and plug them
                    
                    write(pipe_farm[i][1], test, buffer); //write test to each pipe
                    close(pipe_farm[i][1]); //Plug the pipes

                }

                for(i = kid_count; i < (2 * kid_count); i++) {
                    
                    close(pipe_farm[i][1]); //plug the rest of the pipes
                }

                
                for(i = 0, i < kid_count; i++){ //Wait for all kids
                    wait(NULL); //Wait for kids (DO NOT LET THEM ZOMBIFY!)
                }

                for(i = 0; i < kid_count; i++) { // TEMP REMOVE
                    fseek(tmp_files[i], 0, SEEK_SET);
                    fgets(buffer, 8, tmp_files[i]);
                    printf("%s\n", buffer);
                }
                exit(1);
        }
    }
    
 }
