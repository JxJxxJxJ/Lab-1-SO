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
  int fd_0, fd_1;

  // seteo redirecciones
  if (redirIN != NULL) {
    fd_0 = open(redirIN, O_RDONLY); // abro el archivo en modo lectura
    dup2(fd_0, STDIN_FILENO);
    close(fd_0);
  }
  if (redirOUT != NULL) {
    fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd_1, STDOUT_FILENO);
    close(fd_1);
  }

  // execution
  if (builtin_is_internal(sc)) {
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
  if (builtin_alone(apipe)) {
    builtin_run(pipeline_front(apipe));
    return;
  }

  if (!pipeline_is_empty(apipe)) {
    // length de apipe
    int pl_length = pipeline_length(apipe);

    /*
    Arreglo bidimensional, que representa la i-ésima pipe creada, con sus
    respectivos pipe[i][0] como punta de lectura y pipe[i][1] como punta de
    escritura
    */
    int number_of_pipes = pl_length - 1;
    int ith_pipe[number_of_pipes][2];
    int *pids = malloc(sizeof(int) * pl_length);

    // creamos todas las pipes
    for (int i = 0; i < number_of_pipes; i++) {
      pipe(ith_pipe[i]);
    }

    for (int i = 0; i < pl_length; i++) {

      int pid = fork();

      if (pid < 0) {
        perror("fork failed\n");
        exit(EXIT_FAILURE);
      } else if (pid == 0) { // child process

        if (0 < i) {
          dup2(ith_pipe[i - 1][0],
               STDIN_FILENO); // conecto el stdin a la punta de lectura del pipe
                              // anterior
        }
        if (i < pl_length - 1) {
          dup2(ith_pipe[i][1], STDOUT_FILENO); // conecto el stdout a la punta
                                               // de escritura del pipe actual
        }

        // cierro todos los fd de las pipes creadas, al haber hecho las
        // redirecciones, ya no son necesarias
        for (int k = 0; k < number_of_pipes; k++) {
          close(ith_pipe[k][0]);
          close(ith_pipe[k][1]);
        }

        // ejecucion del comando
        scommand sc = pipeline_front(apipe);
        execute_scommand(sc);
        perror("execution failed\n");
        exit(EXIT_FAILURE);
      } else { // father process
        // cierro los pipes en el proceso padre
        // i == 0 -> no se creo una pipe anterior
        // i == number_of_pipes -> no se creo una pipe posterior

        if (0 < i) {
          close(ith_pipe[i - 1][0]); // Cerrar pipe anterior
          close(ith_pipe[i - 1][1]); // Cerrar pipe anterior
        }
        if (i < number_of_pipes) {
          close(ith_pipe[i][0]); // Cerrar pipe actual
          close(ith_pipe[i][1]); // Cerrar pipe actual
        }

        pids[i] = pid; // guardo el pid para que el proceso padre pueda esperar
                       // a su hijo
        pipeline_pop_front(apipe);
      }
    }

    if (pipeline_get_wait(apipe)) {
      for (int i = 0; i < pl_length; i++)
        waitpid(pids[i], NULL,
                0); // cada proceso espera a su proceso hijo correspondiente
    }

    free(pids);
  }
}
