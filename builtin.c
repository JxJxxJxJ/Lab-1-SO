#include "builtin.h"
#include "command.h"
#include <assert.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum de los comandos que mybash acepta
// CMD_COUNT es la cantidad de comandos aceptados hasta ahora por la shell
typedef enum { CMD_CD, CMD_HELP, CMD_EXIT, CMD_COUNT } Command;

static void handle_cd(scommand sc) { printf("Estoy ejecutando cd\n"); }
static void handle_help(scommand sc) { printf("Estoy ejecutando help\n"); }
static void handle_exit(scommand sc) { printf("Estoy ejecutando exit\n"); }

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
  void (*handler)(scommand); // Puntero a la funcion que maneja el comando
  const char *help;          //  Descripcion del comando (--help)
} CMD_DATA;

typedef struct {
  const char *name;    // Nombre del comando
  const CMD_DATA data; // Data del comando
} CMD_ENTRY;

// Un arreglo global de CMD_ENTRY.
// Este arreglo lleva el registro de todos los comandos builtin de myBash
// Para agregar un comando al myBash, debo agregar una entrada al arreglo y
// proveer un puntero a una funcion handle valida
CMD_ENTRY commands_registry[CMD_COUNT] = {
    {"cd", {handle_cd, "Cambiar de directorio"}},
    {"help", {handle_help, "Nuestro proyecto de SO"}},
    {"exit", {handle_exit, "Es para salir de la sesion"}},
};

// Devuelva un puntero a una GQueue populado con los nombres gstringuificados de
// CMD_ENTRY commands_registry
static GQueue *init_gq_command_table(void) {
  GQueue *gq_command_table = g_queue_new();
  GString *gstr_name = NULL;
  // Tomo los nombres de los comandos en commands_registry, los gstringuifico y
  // los meto a la GQueue* gq_command_table
  for (size_t i = 0; i < CMD_COUNT; i++) {
    gstr_name = g_string_new(commands_registry[i].name);
    // Push tail es obligatorio porque mantiene el orden con commands_registry
    g_queue_push_tail(gq_command_table, gstr_name);
    gchar *ret_code = g_string_free(gstr_name, true);
    assert(ret_code == NULL);
  }
  return gq_command_table;
}

// Esta funcion inicializa automaticamente a la funcion static void init
// gq_command_table(CMD_ENTRY entries[])
// en caso de ser necesario
// static void ensure_initialized(void) {
//   static bool initialized = false;
//   if (!initialized) {
//     init_gq_command_table(commands_registry);
//     initialized = true;
//   }
// }

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
  // flags) y armo un gstring con eso
  GString *gstr_scommand = g_string_new(scommand_front(cmd));

  // Busco en la tabla/gqueue CMD_ENTRY gq_command_table.
  // Como es un GQueue puedo usar g_queue_index.
  int indx = g_queue_index(gq_command_table, gstr_scommand);

  if (indx != -1) {
    found = true;
    // DEBUG
    // printf("EL comando ES interno\n. Indx es: %d\n Se leyo generado es:
    // %s\n",
    //        indx, scommand_to_string(cmd));
  }

  // Destruyo la gq_command_table con la funcion para liberar cada elemento (son
  // gstrings, los libero con g_string_free)
  // g_queue_free_full(gq_command_table, g_free);

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
  bool b = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
  return b;
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
  GString *gstr_scommand = g_string_new(scommand_front(cmd));
  printf("El comando leido por builtin run es: %s \n", gstr_scommand->str);

  // Busco en la tabla/gqueue CMD_ENTRY gq_command_table.
  // Como es un GQueue puedo usar g_queue_index.
  gint indx = g_queue_index(gq_command_table, gstr_scommand);

  if (indx != -1) {
    // O assert(indx != -1)?
    // Ejecuta la funcion del comando con el scomando cmd
    commands_registry[indx].data.handler(cmd);
  }

  printf("El indx devuelto es: %d\n", indx);

  // Destruyo la gq_command_table con la funcion para liberar cada elemento (son
  // gstrings, los libero con g_string_free)
  // g_queue_free_full(gq_command_table, g_free);
}
