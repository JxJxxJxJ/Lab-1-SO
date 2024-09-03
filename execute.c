#include "execute.h"
#include "builtin.h"
#include "command.h"
#include <assert.h>
#include <fcntl.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void execute_scommand(scommand sc) {
  char *redirIN = scommand_get_redir_in(sc);
  char *redirOUT = scommand_get_redir_out(sc);
  int file_fd_0, file_fd_1;

  // seteo redirecciones
  if (redirIN != NULL){
    file_fd_0 = open(redirIN, O_RDONLY); // abro el archivo en modo lectura
    dup2(file_fd_0, STDIN_FILENO);
    close(file_fd_0);
  }
  if (redirOUT != NULL){
    file_fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(file_fd_1, STDOUT_FILENO);
    close(file_fd_1);
  }

  // execution
  if (builtin_is_internal(sc)){
    builtin_run(sc);
    return; // ejecuto builtin y salgo de execute_scommand
  }

  unsigned int sc_length = scommand_length(sc);
  char **argvs = malloc((sc_length + 1) * sizeof(char *)); // +1 para NULL
  for (unsigned int i = 0; i < sc_length; i++) {
    unsigned int length_arg = strlen(scommand_front(sc)) + 1; // +1 para '\0'
    argvs[i] = malloc(length_arg * sizeof(char));
    strcpy(argvs[i], scommand_front(sc));
    scommand_pop_front(sc);
  }

  argvs[sc_length] = NULL; // requerimiento de execvp

  execvp(argvs[0], argvs);
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
void execute_pipeline(pipeline apipe) {
  assert(apipe != NULL);
    if(builtin_alone(apipe)){
        builtin_run(pipeline_front(apipe));
        return;
    }

    if (!pipeline_is_empty(apipe)) {
        // length de apipe
        int pl_length = pipeline_length(apipe);


        // file descriptors y temporales, para conectar las salidas con las
        // entradas.
        int fds[2];
        int temp[2];
        int *pids = malloc(sizeof(int) * pl_length);

        for (int i = 0; i < pl_length; i++) {

          if (i > 0) {
              // guardo los file descriptors del proceso en temp para luego
              // reconectarlos con el siguiente proceso (no lo hago si soy el primer
              // proceso)
              temp[0] = fds[0];
              temp[1] = fds[1];
          }

          if (i < pl_length - 1) {
              pipe(fds);
          }

          int pid = fork();

          if (pid < 0) {
              perror("fork failed\n");
              exit(EXIT_FAILURE);
          } 
          else if (pid == 0){ // child process

              if (i > 0){
                dup2(temp[0], STDIN_FILENO); // conecto el stdin a la punta de lectura del pipe anterior
                close(temp[0]);
                close(temp[1]);
              }
              if (i < pl_length - 1){
                dup2(fds[1], STDOUT_FILENO); // conecto el stdout a la punta de escritura del pipe actual
                close(fds[0]);
                close(fds[1]);
              }
              
              // ejecucion del comando
              scommand sc = pipeline_front(apipe);
              execute_scommand(sc);
              perror("execution failed\n");
              exit(EXIT_FAILURE);
          } 
          else{ // father process
              if (i > 0) {
                close(temp[0]);
                close(temp[1]);
              }

              pids[i] = pid; // guardo el pid para que el proceso padre pueda esperar a su hijo
              pipeline_pop_front(apipe);
          }
        }

        if (pipeline_get_wait(apipe)) {
          for (int i = 0; i < pl_length; i++)
              waitpid(pids[i], NULL, 0); // cada proceso espera a su proceso hijo correspondiente
        }

        free(pids);
    }
}
