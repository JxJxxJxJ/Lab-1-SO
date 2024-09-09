#include "execute.h"
#include "builtin.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <glib.h>
#include <signal.h>
#include "command.h"
#include "tests/syscall_mock.h"

static void execute_external_command(scommand sc) {
  // Precondiciones
  assert(sc != NULL);
  assert(!scommand_is_empty(sc));

  unsigned int sc_length = scommand_length(sc);
  char **args_sc = malloc((sc_length + 1) * sizeof(char *)); // +1 para NULL

  for (unsigned int i = 0; i < sc_length; i++) {
    unsigned int length_arg = strlen(scommand_front(sc)) + 1; // +1 para '\0'
    args_sc[i] = malloc(length_arg * sizeof(char));
    strcpy(args_sc[i],scommand_front(sc));
    scommand_pop_front(sc);
  }
  args_sc[sc_length] = NULL; // Requerimiento de execvp()
 
  // Seteo de redirección de la entrada estándar
  char *redirIN = scommand_get_redir_in(sc);     
  if (redirIN != NULL) {
    int fd = open(redirIN,O_RDONLY,S_IRWXU); // Abre el archivo en modo lectura
    if(fd < 0){
      fprintf(stderr,"Error al abrir el archivo de input: %s\n",redirIN);
      exit(EXIT_FAILURE);
    }
    dup2(fd,STDIN_FILENO);  // Redirecciona la entrada estándar al archivo abierto
    close(fd);
  }
  
  // Seteo de redirección de la salida estándar
  char *redirOUT = scommand_get_redir_out(sc);
  if(redirOUT != NULL) {
    int fd = open(redirOUT,O_WRONLY | O_CREAT,S_IRWXU); // Abre el archivo de salida, creándolo si 
                                                        // no existe, y trunca su contenido
    if(fd < 0) {
      fprintf(stderr,"Error al abrir el archivo de output: %s\n",redirOUT);
      exit(EXIT_FAILURE);
    }
    dup2(fd,STDOUT_FILENO); // Redirecciona la salida estándar al archivo abierto
    close(fd);
  }

  // Ejecución del comando externo
  execvp(args_sc[0], args_sc);
  perror("Error al intentar ejecutar el comando");

  // Libera la memoria si execvp() falla 
  for(unsigned int i = 0; i < sc_length; i++) {
    free(args_sc[i]);
  }
  free(args_sc);

  exit(EXIT_FAILURE);
}


static void execute_one_command(pipeline apipe) {
  // Precondiciones
  assert(apipe != NULL);
  assert(pipeline_length(apipe) == 1);

  int pid = fork(); // Crea un nuevo proceso

  if(pid < 0) { // Error en el fork
    perror("fork() ha fallado.\n");
    exit(EXIT_FAILURE);
  }
  else if(pid == 0) { // PROCESO HIJO
    execute_external_command(pipeline_front(apipe)); 
  }
  else {  // PROCESO PADRE
    if(pipeline_get_wait(apipe)) {
      wait(NULL); // Si el pipeline pide esperar, espera a que termine el hijo 
    }
    pipeline_pop_front(apipe);
  }
}


static void execute_two_commands(pipeline apipe) {
  // Precondiciones
  assert(apipe != NULL);
  assert(pipeline_length(apipe) == 2);

  int fd[2];  // Crea un arreglo de file descriptors para el pipe
  pipe(fd); // Crea el pipe entre los procesos 

  int pid1 = fork();  // Crea el primer proceso
  if(pid1 < 0) {  // Error en el fork
    perror("fork() ha fallado.\n");
    exit(EXIT_FAILURE);
  }
  if(pid1 == 0) { // PRIMER HIJO
    dup2(fd[1],STDOUT_FILENO);  // Redirecciona la salida estándar al pipe
    close(fd[0]); // Cierra la punta de lectura del pipe
    close(fd[1]); // Cierra la punta de escritura del pipe
    execute_external_command(pipeline_front(apipe));  // Ejecuta el primer comando
  }
  pipeline_pop_front(apipe);  // Primer comando afuera

  int pid2 = fork();  // Crea el segundo proceso
  if(pid2 == 0) { // SEGUNDO HIJO
    dup2(fd[0],STDIN_FILENO); // Redirecciona la entrada estándar al pipe 
    close(fd[0]); // Cierra la punta de lectura del pipe
    close(fd[1]); // Cierra la punta de escritura del pipe
    execute_external_command(pipeline_front(apipe));  // Ejecuta el segundo comando
  }
  else {  // PADRE
    close(fd[0]); // Cierra la punta de lectura del pipe
    close(fd[1]); // Cierra la punta de escritura del pipe
    pipeline_pop_front(apipe);  // Segundo comando afuera
  }

  if(pipeline_get_wait(apipe)) {
    waitpid(pid1,NULL,0); // Espera a que termine el primer hijo
    waitpid(pid2,NULL,0); // Espera a que termine el segundo hijo
  }
}


static void execute_n_commands(pipeline apipe) {
  // Precondiciones
  assert(apipe != NULL);
  assert(pipeline_length(apipe) > 2);

  int pl_length = pipeline_length(apipe);
  int current_fd[2];  // FDs para el pipe del proceso actual
  int previous_fd[2]; // FDs para el pipe del proceso anterior
  int *pid_table = malloc(pl_length * sizeof(int)); // Tabla para guardar los PIDs de los hijos

  for(int i = 0; i < pl_length; i++) {
    if(i > 0) {  // Si no es el primer proceso...
      previous_fd[0] = current_fd[0]; // Guardo los file descriptors del proceso anterior para luego 
      previous_fd[1] = current_fd[1]; // reconectarlos con el siguiente proceso  
    }

    if(i < pl_length - 1) {  // Si no es el último proceso...
      pipe(current_fd); // Crea un pipe nuevo
    }

    int pid = fork(); // Crea un nuevo proceso
    if(pid < 0) {
      perror("fork() ha fallado.\n");
      exit(EXIT_FAILURE);
    } 
    else if(pid == 0) { // PROCESO HIJO
      if(i > 0) { // Si no es el primer proceso...
        dup2(previous_fd[0],STDIN_FILENO); // Conecto el stdin a la punta de lectura del pipe anterior
        close(previous_fd[0]);
        close(previous_fd[1]);
      }
      if(i < pl_length - 1) { // Si no es el último proceso...
        dup2(current_fd[1],STDOUT_FILENO); // Conecto el stdout a la punta de escritura del pipe actual
        close(current_fd[0]);
        close(current_fd[1]);
      }
              
      // Ejecución del comando externo
      execute_external_command(pipeline_front(apipe));
    } 
    else { // PROCESO PADRE
      if(i > 0) {
        // Cierra los FDs del proceso anterior
        close(previous_fd[0]);
        close(previous_fd[1]);
      }

      pid_table[i] = pid; // Guardo el PID para que el proceso padre pueda esperar a su hijo
      pipeline_pop_front(apipe);  // Elimina el comando actual de la pipeline
    }
  }

  // Si hay que esperar a los hijos, usa 'waitpid' para cada proceso
  if(pipeline_get_wait(apipe)) {
    for (int i = 0; i < pl_length; i++)
      waitpid(pid_table[i], NULL, 0); // Cada proceso espera a su proceso hijo correspondiente
  }

  // Libera la memoria de la tabla de PIDs
  free(pid_table);
}


void execute_pipeline(pipeline apipe) {
  assert(apipe != NULL);

  if (pipeline_is_empty(apipe)) { // Si el pipeline está vacío, no hay nada que ejecutar
    printf("La pipeline está vacía.\n");
    return;
  }
  else if (builtin_alone(apipe)) {  // Si el pipeline tiene un solo builtin, lo ejecutamos directamente
    builtin_run(pipeline_front(apipe));
  }
  else {  // Si el pipeline tiene comandos externos...
    if(pipeline_length(apipe) == 1) { // Si hay un solo comando, lo ejecuta
      execute_one_command(apipe);
    }
    else if(pipeline_length(apipe) == 2) { // Si hay dos comandos, los ejecuta conectados con una pipe
      execute_two_commands(apipe);
    }
    else {  // Si hay n comandos (> 2), los ejecuta conectando una pipe entre cada uno
      execute_n_commands(apipe);
    }
  }
}
/*
 * Ejecuta un pipeline, identificando comandos internos, forkeando, y
 *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
 *   de ejecución.
 *   apipe: pipeline a ejecutar
 * Requires: apipe!=NULL
 */