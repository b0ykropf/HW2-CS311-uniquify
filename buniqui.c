/* 
*    Author: Byron Kropf
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
#include <ctype.h>

#define WORD_SIZE 100
#define MAX_TOKEN 2000

#define FALSE 0
#define TRUE !FALSE

int main(int argc,char *argv[]){

    if( argc != 2){ //Check to see if enough arguments were given
        err_exit("ERROR: You did not give 1 arguments.");
    }

    int kid_count = atoi(argv[1]); //stores first argument as number of children to create

    int aim = 0;
    int length;
    int status;
int test0;
int test1;

    int** pipe_farm = (int**) malloc(sizeof(int*) * (kid_count * 2));

    pid_t* kid_id = (pid_t*) malloc(sizeof(pid_t) + sizeof(pid_t) * kid_count);

    char* token = (char*) malloc(sizeof(char) * MAX_TOKEN);

    FILE** pipe_point = (FILE**) malloc(sizeof(FILE*) * kid_count * 2);

    for(int i = 0; i < (kid_count * 2); i++){
        pipe_farm[i] = malloc(sizeof(int) * 2);
        if( pipe(pipe_farm[i]) == -1 ) { //create pipes in pipe_farm
            err_exit("ERROR: Problem creating pipe");
        }
        else{
        printf("%s %d \n", "Pipe plumbed: ", i);
        }
test0 = pipe_farm[i][0];
test1 = pipe_farm[i][1];
printf("%s %d %d \n","Pipe_farm[i] [0] [1] ",test0,test1);
    }
    for(int i = 0; i < kid_count; i++){
        if((pipe_point[i] = fdopen(pipe_farm[i][1],"w")) == NULL ) { //pointer to write to ith sort pipe
            err_exit("ERROR: Problem creating sort pointer");
        }
        else{
        printf("%s %d \n", "Pointer Pointed: ", i);
        }
    }
    for(int i = kid_count; i < (kid_count * 2); i++){
               if((pipe_point[i] = fdopen(pipe_farm[i][0],"r")) == NULL ) { //pointer to read to ith sup pipe
            err_exit("ERROR: Problem creating sup pointer");
        }
        else{
        printf("%s %d \n", "Pointer Pointed: ", i);
        }
    }
printf("Going into Switch \n");  
    for( int i = 0; i <= kid_count; i++ ) {//spawns argv[1]+1 kids, +1 is suppressor
       
        switch( ( kid_id[i] = fork() ) ) { //Spawn Children (Pray that they do not zombify)

            case 0:
                
                if(i == kid_count){ //Special instructions for suppressor
                    printf("%s %d \n", "Hi my name is KID: ", kid_id[i]);
                    for( int j = 0; j < kid_count; j++){ //plug uneeded pipes
                        if(close(pipe_farm[j][0]) != 0){
                            err_exit("ERROR: closing read end of non-sup-pipe\n");
                        }
                        else{
                            printf("%s %d \n", "SUP:close read non-sup-pipe", j);
                        }
                        if(close(pipe_farm[j][1]) != 0){
                            err_exit("ERROR: closing write end of non-sup-pipe\n");
                        }
                        else{
                            printf("%s %d \n", "SUP:close write non-sup-pipe", j);
                        }
                    }//end plugging non-sup-pipes

                    for( int j = kid_count; j < (2 * kid_count); j++){ //plug write ends
                        if( close(pipe_farm[j][1]) != 0) {
                            err_exit("ERROR:Problem plugging write end of sup-pipe");
                        }
                        else{
                            printf("%s %d \n", "SUP:close write sup-pipe", j);
                        }    
                    }// end plugging write ends

//SUPPRESSOR CODE GOES HERE
                free(token);
                free(pipe_point);
                free(pipe_farm);

                    _exit(1);
                   // break;
                }//End special instructions for suppressor

                if(i != kid_count){
                    printf("%s %d \n", "Hi my name is KID: ", kid_id[i]);
                    for( int j = 0; j < kid_count; j++){ //plug write ends in pipes
                        if(close(pipe_farm[j][1]) != 0){
                            err_exit("ERROR: closing write end of sort pipe.\n");
                        }
                        else{
                            printf("%s %d \n", "SORT:close write sort pipe.", j);
                        }
                        if(j != i){ //if not my pipe also close read end
                            if(close(pipe_farm[j][0]) != 0){
                                err_exit("ERROR: closing read end not my pipe.\n");
                            }
                            else{
                            printf("%s %d \n", "SORT:close read not my pipe.", j);
                            }
                        }
                    }//end plugging write ends and uneeded pipes
printf("Checkpoint 1 \n");
                    for( int j = kid_count; j < (2 * kid_count); j++){ //plug read ends
                        if( close(pipe_farm[j][0]) != 0) {
                            err_exit("ERROR:Problem plugging read end of sup-pipe");
                        }
                        else{
                            printf("%s %d \n", "SORT:close read sup-pipe", j);
                        }
                        if(j != (kid_count + i)){//if not my pipe also close write
                            if(close(pipe_farm[j][1]) != 0){
                                err_exit("ERROR: closing write end not my pipe.\n");
                            }
                            else{
                            printf("%s %d \n", "SORT:close write not my pipe.", j);
                            }
                        }    
                    }// end plugging read ends and uneeded pipes
test0 = pipe_farm[i][0];
test1 = pipe_farm[i][1];
printf("%s %d %d \n","Pipe_farm[i] [0] [1] ",test0,test1);
printf("Checkpoint 2 \n");
                    dup2(test0, 0);// makes sup pipe stdout
                    dup2(test1, 1); // makes in pipe stdin
printf("Checkpoint 3 \n");
                    close(pipe_farm[i][0]);
                    close(pipe_farm[i][1]);
                    execl("/usr/bin/sort", "sort", NULL); // make child into sort
                    printf("%s %d \n", "exec done ", i);
                free(token);
                free(pipe_point);
                free(pipe_farm);
                    _exit(1);
                //end sorter code
                    break;

            default:

                for(int i = 0; i < (2 * kid_count); i++) { //Parent never reads from pipe
	            if( close(pipe_farm[i][0]) == -1 ) {
                        err_exit("ERROR: Problem closeing read end of pipes for parent");
                    }
                    else{
                        printf("%s%d\n","Parent close pipe read: ",i);
                    } 
                }
               
                for(i = kid_count; i < (2 * kid_count); i++) {//Parent does not write to suppressor
	            if( close(pipe_farm[i][1]) == -1 ) {
                        err_exit("ERROR: Problem closeing write end sup pipes for parent");
                    }
                    else{
                        printf("%s%d\n","Parent close write to sup: ",i);
                    }
                }
             
                while(fscanf(stdin,"%s",token) != EOF) {
                    length = strlen(token);
                    add_cr(token,length);
                    lower_case(token, length);
                    fputs(token, pipe_point[aim]);
printf("%s\n",token);
                    if(aim == (kid_count - 1)){ // if we used last pipe, wrap around
                        aim = -1;
                    }
                    aim++;
                    printf("%s %d \n","Traffic cop is: ",aim);
                }
                if(fscanf(stdin,"%s",token) == EOF){
                    printf("parent reached EOF");
                }
                
                //break;
            //end default
        }//end switch
    }//end for        
            for(int i = 0; i < kid_count; i++) {//Parent finished writing
	            if( close(pipe_farm[i][1]) == -1 ) {
                        err_exit("ERROR: Problem closeing write end sort pipes for parent");
                    }
                    else{
                        printf("%s%d\n","Parent close write to sort: ",i);
                    }
                } //finished closing last pipes

                for(int i = 0; i < (2 * kid_count); i++){ //free pipes in parent
          
                    free(pipe_farm[i]);
                    printf("%s %d \n","Parent free pipe: ", i);
                }
                free(token);
                free(pipe_point);
                free(pipe_farm);

                while (waitpid(-1,&status,WNOHANG) > 0){
                    printf("Child exited");
                }
                

                exit(1);

    }//end for   
}//end main
