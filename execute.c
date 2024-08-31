#include "execute.h"
#include "builtin.h"
#include "command.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

static void execute_scommand(scommand sc){
    char *redirIN =  scommand_get_redir_in(sc);
    char *redirOUT =  scommand_get_redir_out(sc);
    int file_fd_0, file_fd_1;

    // setting redirections
    if(redirIN != NULL){
        file_fd_0 = open(redirIN, O_RDONLY); // open input file in Reading mode
        dup2(file_fd_0, STDIN_FILENO);
        close(file_fd_0);
    }
    if(redirOUT != NULL){
        file_fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, 0644); // open output file | create if it doesnt exists | truncate it if it already exists 
        dup2(file_fd_1, STDOUT_FILENO);
        close(file_fd_1);
    }


    // execution
    unsigned int sc_length = scommand_length(sc);
    char *argvs[sc_length+1];
    for(unsigned int i=0; i<sc_length ;i++){
        argvs[i] = scommand_front(sc);
    }

    argvs[sc_length] = NULL; // requirement for execvp

    execvp(argvs[0],argvs); // if this goes well, lines under this wont be readed
    perror("Error executing command");
    exit(EXIT_FAILURE);
}


/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */
void execute_pipeline(pipeline apipe){
    assert(apipe != NULL);
    if(!pipeline_is_empty(apipe)){
        // length de apipe
        int pl_length = pipeline_length(apipe);

        if(builtin_alone(apipe)){
            builtin_run(pipeline_front(apipe));
        }
        else if(pl_length > 0){
            for(int i=0; i < pl_length;i++){

                int pid = fork();

                if(pid < 0){
                    perror("fork failed\n");
                    exit(EXIT_FAILURE);
                }                    
                else if(pid == 0){ // child process
                    scommand sc = pipeline_front(apipe);
                    execute_scommand(sc);
                    perror("execution failed\n");
                    exit(EXIT_FAILURE);
                }
                else{   // father process
                    if(pipeline_get_wait(apipe)){
                        waitpid(pid,NULL,0); // wait for the child procces just created and not for any other ones
                    }
                    pipeline_pop_front(apipe);
                }
            }
        }
    }
}

