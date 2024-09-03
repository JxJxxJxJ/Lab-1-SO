#include "builtin.h"
#include "command.h"
#include <assert.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir

// Enum de los comandos que mybash acepta
// CMD_COUNT es la cantidad de comandos aceptados hasta ahora por la shell
typedef enum { CMD_CD, CMD_HELP, CMD_EXIT, CMD_COUNT } Command;

static void handle_cd(scommand sc) {
  // Planteo dos casos: uno en el que solo se coloca el cd sin dirección y otro
  // donde se le da una dirección específica.

  // Si el tamaño del scommand es 1, significa que se trata del cd solo sin
  // dirección.
  if (scommand_length(sc) == 1) {
    chdir(getenv("HOME")); // La variable 'HOME' está configurada para apuntar
                           // al directorio personal del usuario actual,
                           // entonces getenv accede al valor de dicha variable
                           // y me transporta a esa dirección.
  } else { // Si el tamaño del scommand es != 1, significa que el cd viene
           // acompañado de una dirección.
    scommand_pop_front(
        sc); // Popeo el cd para quedarme solamente con la dirección.
    char *path =
        scommand_to_string(sc); // Guardo la dirección en la variable path.
    int k = chdir(path); // Intento llegar a la dirección con chdir (caso
                         // exitoso cuando k == 0).

    // Si k == -1, entonces no se puede acceder a la ruta entregada.
    if (k == -1) {
      printf("myBash: cd: %s: El archivo o directorio no existe.\n", path);
    }
  }
}

static void handle_help(scommand sc) {
  printf("«myBash» v1.0, por :(){ :|:& };: (g-01)\n\n");
  printf("Los autores que colaboraron para la creación del mismo son: \n- Juan "
         "Cruz Hermosilla Artico\n- Gaspar Saenz Valiente\n- Exequiel "
         "Trinidad\n- Fernando Cabrera Luque\n\n");
  printf("Los comandos internos de este bash son:\n");
  printf(
      "'cd <path>' permite desplazarse entre los directorios del sistema.\n");
  printf("'help' muestra los comandos internos del bash e información del "
         "mismo (¡USTED ESTÁ AQUÍ!).\n");
  printf("'exit' cierra el bash actual.\n");
}

static void handle_exit(scommand sc) {
  printf("¡Adiós!\n");
  exit(EXIT_SUCCESS);
}

// Cada comando <comando> tendra una funcion void asociada <handle_comando> y
// una descripcion de lo que hace en help

// uso const para decir que los datos son solo de lectura
/* void * (*function)();
 * “declare a function pointer named function, which points to a function that
 * takes an unspecified number of arguments, then returns a void *"
 *
 * <tipo_de_output> (*<nombre_de_funcion>) (<tipo_de_argumento_de_la_funcion>)
 */

// Todo comando debera tener una funcion que se encargue de ejecutar la tarea
// del comando y una descripcion obtenible al hacer <comando> --help
typedef struct {
  void (*handler)(scommand); // Funcion que maneja el comando
  const char *help;          // Descripcion del comando
} CMD_DATA;                  //
                             //
typedef struct {             //
  const char *name;          // Nombre del comando
  const CMD_DATA data;       // Data del comando
} CMD_ENTRY;

// Un arreglo global de CMD_ENTRY.
// Este arreglo lleva el registro de todos los comandos builtin de myBash
// Para agregar un comando al myBash, debo agregar una entrada al arreglo y
// proveer un puntero a una funcion handle valida
CMD_ENTRY commands_registry[CMD_COUNT] = {
    {"cd", {handle_cd, "Cambiar de directorio"}},
    {"help", {handle_help, "Nuestro proyecto de SO"}},
    {"exit", {handle_exit, "Es para salir de la sesión"}},
};

// Devuelva un puntero a una GQueue populado con los nombres gstringuificados de
// CMD_ENTRY commands_registry
static GQueue *init_gq_command_table(void) {
  GQueue *gq_command_table = g_queue_new();
  // Tomo los nombres de los comandos en commands_registry, los gstringuifico y
  // los meto a la GQueue* gq_command_table
  for (size_t i = 0; i < CMD_COUNT; i++) {
    char *name_pointer =
        malloc((strlen(commands_registry[i].name) + 1) * sizeof(char));

    if (name_pointer == NULL) {
      printf("Error al asignar memoria.\n");
      exit(EXIT_FAILURE);
    }

    strcpy(name_pointer, commands_registry[i].name);
    // Push tail es obligatorio porque mantiene el orden con commands_registry
    g_queue_push_tail(gq_command_table, name_pointer);
  }

  return gq_command_table;
}

// ------------------------------------------------------------------------
// | En este punto mi GQueue* command_table deberia tener                 |
// | los comandos que acepta MyBash                                       |
// ------------------------------------------------------------------------

/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */
bool builtin_is_internal(scommand cmd) {
  assert(cmd != NULL);
  bool found = false;
  // Inicializo la gq_command_table para realizar la busqueda ALOCA MEMORIA
  GQueue *gq_command_table = init_gq_command_table();

  // Tomo lo que el scommand cmd tenia al frente (el cd, el help, etc sin sus
  // flags) y armo un string con eso
  char *str_scommand = scommand_front(cmd);

  // Busco manualmente en la GQueue el str_scommand
  for (unsigned int i = 0; i < g_queue_get_length(gq_command_table) && !found;
       i++) {
    char *name_pointer = (char *)g_queue_peek_nth(gq_command_table, i);
    if (strcmp(name_pointer, str_scommand) == 0) {
      found = true;
      // printf("El comando es interno. Indx es: %d\n Se leyó generado es:
      // %s\n", i, scommand_to_string(cmd));
    }
  }

  // if (!found) { // No hace falta imprimirlo. Si no es builtin puede ser
  // binary
  //   printf("%s: no se encontró la orden\n", str_scommand);
  // }

  // Destruyo la gq_command_table con la funcion para liberar cada elemento (son
  // strings, los libero con free)
  g_queue_free_full(gq_command_table, free);
  return found;
}

/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 */
bool builtin_alone(pipeline p) {
  assert(p != NULL);
  return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
}

/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */
void builtin_run(scommand cmd) {
  assert(builtin_is_internal(cmd));
  // Inicializo la gq_command_table para realizar la busqueda ALOCA MEMORIA
  GQueue *gq_command_table = init_gq_command_table();

  // Obtengo el nombre del comando
  char *str_scommand = scommand_front(cmd);
  // printf("El comando leído por builtin run es: %s \n", str_scommand);

  // Busco manualmente en la GQueue str_scommand
  int indx = -1;
  bool found = false;
  for (unsigned int i = 0; i < g_queue_get_length(gq_command_table) && !found;
       i++) {
    char *name_pointer = g_queue_peek_nth(gq_command_table, i);
    if (strcmp(name_pointer, str_scommand) == 0) {
      indx = i;
      found = true;
    }
  }

  if (indx != -1) {
    // Ejecuta la funcion del comando con el scomando cmd
    commands_registry[indx].data.handler(cmd);
  }

  // printf("El indx devuelto es: %d\n", indx);

  g_queue_free_full(gq_command_table, free);
}
