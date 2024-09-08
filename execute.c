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

// ------------------ VERSION 2.1 (Ahora para n comandos (también sin array bidimensional)) ------------------
// Es una versión donde se modularizan ciertas funciones para mayor prolijidad en el código pero
// que conserva la lógica de la 2.0. Se renombran algunas variables o prints de error, se forman 
// nuevas funciones, etc.

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



// -------------------------------------------- VERSION 2.0 (Ahora para n comandos (sin array bidimensional))------------------------------------------------
// Junto a las funciones set_up_redirections() y manual_exec() en una que abarque la ejecución del comando externo
// en general
// static void execute_external_cmd(scommand sc) {
//   unsigned int sc_length = scommand_length(sc);
//   char **argvs = malloc((sc_length + 1) * sizeof(char *)); // +1 para NULL

//   for (unsigned int i = 0; i < sc_length; i++) {
//     unsigned int length_arg = strlen(scommand_front(sc)) + 1; // +1 para '\0'
//     argvs[i] = malloc(length_arg * sizeof(char));
//     strcpy(argvs[i], scommand_front(sc));
//     scommand_pop_front(sc);
//   }
//   argvs[sc_length] = NULL; // requerimiento de execvp

//   // seteo redirecciones
//   char *redirIN = scommand_get_redir_in(sc);
//   if (redirIN != NULL) {
//     int fd_0 = open(redirIN, O_RDONLY, S_IRWXU); // abro el archivo en modo lectura
//     if(fd_0 < 0){
//       fprintf(stderr,"Error al abrir el archivo de input: %s\n", redirIN);
//       exit(EXIT_FAILURE);
//     }
//     dup2(fd_0, STDIN_FILENO);
//     close(fd_0);
//   }

//   char *redirOUT = scommand_get_redir_out(sc);
//   if (redirOUT != NULL) {
//     int fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
//     if(fd_1 < 0){
//       fprintf(stderr,"Error al abrir el archivo de output: %s\n", redirOUT);
//       exit(EXIT_FAILURE);
//     }
//     dup2(fd_1, STDOUT_FILENO);
//     close(fd_1);
//   }

//   // execution
//   execvp(argvs[0], argvs);
//   perror("Error executing command");

//   // liberamos memoria asignada en caso de que execvp falle
//   for (unsigned int i = 0; i < sc_length; i++) {
//     free(argvs[i]);
//   }
//   free(argvs);

//   exit(EXIT_FAILURE);
// }

// // static void set_up_redirections(scommand sc){
// //   char *redirIN = scommand_get_redir_in(sc);
// //   char *redirOUT = scommand_get_redir_out(sc);
// //   int fd_0, fd_1;
        
// //   if (redirIN != NULL) {
// //     fd_0 = open(redirIN, O_RDONLY, S_IRWXU); // abro el archivo en modo lectura
// //     if(fd_0 < 0){
// //       fprintf(stderr,"Error al abrir el archivo de input: %s\n", redirIN);
// //       exit(EXIT_FAILURE);
// //     }
// //     dup2(fd_0, STDIN_FILENO);
// //     close(fd_0);
// //   }
// //   if (redirOUT != NULL) {
// //     fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
// //     if(fd_1 < 0){
// //       fprintf(stderr,"Error al abrir el archivo de output: %s\n", redirOUT);
// //       exit(EXIT_FAILURE);
// //     }
// //     dup2(fd_1, STDOUT_FILENO);
// //     close(fd_1);
// //   }
// // }

// // static void manual_exec(scommand sc) {
// //   unsigned int sc_length = scommand_length(sc);
// //   char **argvs_sc = malloc((sc_length + 1) * sizeof(char *));
// //   for (unsigned int i = 0; i < sc_length; i++) {
// //     argvs_sc[i] = strdup(scommand_front(sc)); 
// //     scommand_pop_front(sc);
// //   }
// //   argvs_sc[sc_length] = NULL; 

// //   execvp(argvs_sc[0], argvs_sc);
// //   perror("Error executing command");

// //   // Liberar la memoria si execvp falla
// //   for (unsigned int i = 0; i < sc_length; i++) {
// //     free(argvs_sc[i]);
// //   }
// //   free(argvs_sc);
// //   exit(EXIT_FAILURE);
// // }

// void execute_pipeline(pipeline apipe) {
//   assert(apipe != NULL);

//   // ¡¡¡¡ESTE CASO ES EL QUE TRABABA Y DEJABA LOS TESTS EN 98%!!!!
//   if (pipeline_is_empty(apipe)) { 
//     printf("La pipeline está vacía.\n");
//     return;
//   }
//   // Si el pipeline tiene un solo builtin, lo ejecutamos directamente
//   else if (builtin_alone(apipe)) {
//     builtin_run(pipeline_front(apipe));
//     return;
//   }
//   else {  // Si el pipeline tiene un comando externo...
//     unsigned int pl_length = pipeline_length(apipe);

//     if (pl_length == 1) {  // Si es el comando externo solo
//       int pid = fork();
//       if (pid < 0) {
//         perror("fork failed\n");
//         exit(EXIT_FAILURE);
//       } 
//       else if (pid == 0) { // Proceso hijo
//         scommand sc = pipeline_front(apipe);
//         execute_external_cmd(sc);
//       }
//       else {
//         if (pipeline_get_wait(apipe)) {
//           wait(NULL);  // Esperar al hijo
//         }
//         pipeline_pop_front(apipe); // Proceso padre       
//       }
//     }
//     else if (pl_length == 2) {  // Si hay dos comandos anexados por una pipe...
//       int fd[2];
//       pipe(fd);

//       int pid1 = fork();
//       if (pid1 < 0) {
//         perror("fork failed\n");
//         exit(EXIT_FAILURE);
//       }
//       if (pid1 == 0) { // Primer hijo
//         dup2(fd[1], STDOUT_FILENO);
//         close(fd[0]); 
//         close(fd[1]);
//         scommand sc1 = pipeline_front(apipe);
//         execute_external_cmd(sc1);
//       }
//       pipeline_pop_front(apipe);

//       int pid2 = fork();
//       if (pid2 == 0) { // Segundo hijo
//         dup2(fd[0], STDIN_FILENO);
//         close(fd[0]); 
//         close(fd[1]);
//         scommand sc2 = pipeline_front(apipe);
//         execute_external_cmd(sc2);
//       }

//       // Proceso padre
//       close(fd[0]);
//       close(fd[1]);
//       pipeline_pop_front(apipe);
    
//       if (pipeline_get_wait(apipe)) {
//         waitpid(pid1, NULL, 0); 
//         waitpid(pid2, NULL, 0); 
//       }
//     }
//     else {  // Acá reservo el caso para generalizar a n pipes
//       int pl_length = pipeline_length(apipe);
//       int *pids = malloc(pl_length * sizeof(int));

//       // file descriptors para conectar las salidas con las entradas.
//       int fd_actual[2];
//       int fd_anterior[2];

//       for (int i = 0; i < pl_length; i++) {

//         if (i > 0) {
//               // guardo los file descriptors del proceso en temp para luego
//               // guardo los file descriptors del proceso anterior para luego
//               // reconectarlos con el siguiente proceso (no lo hago si soy el primer
//               // proceso)
//           fd_anterior[0] = fd_actual[0];
//           fd_anterior[1] = fd_actual[1];
//         }

//         if (i < pl_length - 1) {
//           pipe(fd_actual);
//         }

//         int pid = fork();
//         if (pid < 0) {
//           perror("fork failed\n");
//           exit(EXIT_FAILURE);
//         } 
//         else if (pid == 0){ // child process
//           if (i > 0){
//             dup2(fd_anterior[0], STDIN_FILENO); // conecto el stdin a la punta de lectura del pipe anterior
//             close(fd_anterior[0]);
//             close(fd_anterior[1]);
//           }
//           if (i < pl_length - 1){
//             dup2(fd_actual[1], STDOUT_FILENO); // conecto el stdout a la punta de escritura del pipe actual
//             close(fd_actual[0]);
//             close(fd_actual[1]);
//           }
              
//               // ejecucion del comando
//           scommand sc = pipeline_front(apipe);
//           execute_external_cmd(sc);
//           perror("execution failed\n");
//           exit(EXIT_FAILURE);
//         } 
//         else{ // father process
//           if (i > 0) {
//             close(fd_anterior[0]);
//             close(fd_anterior[1]);
//           }
//           pids[i] = pid; // guardo el pid para que el proceso padre pueda esperar a su hijo
//           pipeline_pop_front(apipe);
//         }

//         if (pipeline_get_wait(apipe)) {
//           for (int i = 0; i < pl_length; i++)
//               waitpid(pids[i], NULL, 0); // cada proceso espera a su proceso hijo correspondiente
//         }
//         free(pids);
//       }
//     }
//   }
// }

// -------------------------------------------- VERSION 1.0 ------------------------------------------------
// static void execute_scommand(scommand sc) {
//   char *redirIN = scommand_get_redir_in(sc);
//   char *redirOUT = scommand_get_redir_out(sc);
//   int fd_0, fd_1;

//   // seteo redirecciones
//   if (redirIN != NULL) {
//     fd_0 = open(redirIN, O_RDONLY, S_IRWXU); // abro el archivo en modo lectura
//     if(fd_0 < 0){
//       fprintf(stderr,"Error al abrir el archivo de input: %s\n", redirIN);
//       exit(EXIT_FAILURE);
//     }
//     dup2(fd_0, STDIN_FILENO);
//     close(fd_0);
//   }
//   if (redirOUT != NULL) {
//     fd_1 = open(redirOUT, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
//     if(fd_1 < 0){
//       fprintf(stderr,"Error al abrir el archivo de output: %s\n", redirOUT);
//       exit(EXIT_FAILURE);
//     }
//     dup2(fd_1, STDOUT_FILENO);
//     close(fd_1);
//   }

//   // execution
//   if (builtin_is_internal(sc)) {
//     builtin_run(sc);
//     return; // ejecuto builtin y salgo de execute_scommand
//   }

//   unsigned int sc_length = scommand_length(sc);
//   char **argvs = malloc((sc_length + 1) * sizeof(char *)); // +1 para NULL
//   for (unsigned int i = 0; i < sc_length; i++) {
//     unsigned int length_arg = strlen(scommand_front(sc)) + 1; // +1 para '\0'
//     argvs[i] = malloc(length_arg * sizeof(char));
//     strcpy(argvs[i], scommand_front(sc));
//     scommand_pop_front(sc);
//   }

//   argvs[sc_length] = NULL; // requerimiento de execvp

//   execvp(argvs[0], argvs);
//   perror("Error executing command");
//   // liberamos memoria asignada en caso de que execvp falle
//   for (unsigned int i = 0; i < sc_length; i++) {
//     free(argvs[i]);
//   }
//   free(argvs);
//   exit(EXIT_FAILURE);
// }

// /*
//  * Ejecuta un pipeline, identificando comandos internos, forkeando, y
//  *   redirigiendo la entrada y salida. puede modificar `apipe' en el proceso
//  *   de ejecución.
//  *   apipe: pipeline a ejecutar
//  * Requires: apipe!=NULL
//  */
// void execute_pipeline(pipeline apipe) {
//   assert(apipe != NULL);
//   if (builtin_alone(apipe)) {
//     builtin_run(pipeline_front(apipe));
//     return;
//   }

//   if (!pipeline_is_empty(apipe)) {
//     // length de apipe
//     int pl_length = pipeline_length(apipe);

//     if(!pipeline_get_wait(apipe)){ 
//       signal(SIGCHLD,SIG_IGN);
//     }
//     /*
//     Arreglo bidimensional, que representa la i-ésima pipe creada, con sus
//     respectivos pipe[i][0] como punta de lectura y pipe[i][1] como punta de
//     escritura
//     */
//     int number_of_pipes = pl_length - 1;
//     int ith_pipe[number_of_pipes][2];
//     int *pids = malloc(sizeof(int) * pl_length);

//     // creamos todas las pipes
//     for (int i = 0; i < number_of_pipes; i++) {
//       pipe(ith_pipe[i]);
//     }

//     for (int i = 0; i < pl_length; i++) {

//       int pid = fork();

//       if (pid < 0) {
//         perror("fork failed\n");
//         exit(EXIT_FAILURE);
//       } 
//       else if (pid == 0) { // child process
        
//         if (0 < i) {
//           dup2(ith_pipe[i - 1][0],
//                STDIN_FILENO); // conecto el stdin a la punta de lectura del pipe
//                               // anterior
//         }
//         if (i < pl_length - 1) {
//           dup2(ith_pipe[i][1], STDOUT_FILENO); // conecto el stdout a la punta
//                                                // de escritura del pipe actual
//         }

//         // cierro todos los fd de las pipes creadas, al haber hecho las
//         // redirecciones, ya no son necesarias
//         for (int k = 0; k < number_of_pipes; k++) {
//           close(ith_pipe[k][0]);
//           close(ith_pipe[k][1]);
//         }

//         // ejecucion del comando
//         scommand sc = pipeline_front(apipe);
//         execute_scommand(sc);
//         perror("execution failed\n");
//         exit(EXIT_FAILURE);
//       } 
//       else { // father process
//         // cierro los pipes en el proceso padre
//         // i == 0 -> no se creo una pipe anterior
//         // i == number_of_pipes -> no se creo una pipe posterior

//         if (0 < i) {
//           close(ith_pipe[i - 1][0]); // Cerrar pipe anterior
//           close(ith_pipe[i - 1][1]); // Cerrar pipe anterior
//         }
//         if (i < number_of_pipes) {
//           close(ith_pipe[i][0]); // Cerrar pipe actual
//           close(ith_pipe[i][1]); // Cerrar pipe actual
//         }

//         pids[i] = pid; // guardo el pid para que el proceso padre pueda esperar
//                        // a su hijo
//       }
//     }

//     pipeline_pop_front(apipe);
//     if (pipeline_get_wait(apipe)) {
//       for (int i = 0; i < pl_length; i++)
//         waitpid(pids[i], NULL,
//                 0); // cada proceso espera a su proceso hijo correspondiente
//     }

//     free(pids);
//   }
// }
