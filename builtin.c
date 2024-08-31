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

static void handle_cd(scommand sc) { printf("Estoy ejecutando cd"); }
static void handle_help(scommand sc) { printf("Estoy ejecutando help"); }
static void handle_exit(scommand sc) { printf("Estoy ejecutando exit"); }

// Cada comando <comando> tendra una funcion void asociada <handle_comando> y
// una descripcion de lo que hace en help
// uso const para decir que los datos son solo de lectura
typedef struct {
  const char *name;          // Nombre del comando
  void (*handler)(scommand); // Puntero a la funcion que maneja el comando
  const char *help;          //  Descripcion del comando (--help)
} CMD_ENTRY;

// Una tabla (array) de CMD_ENTRY
static CMD_ENTRY command_table[CMD_COUNT] = {
    {"cd", handle_cd, "Cambiar de directorio"},
    {"help", handle_help, "Nuestro proyecto de SO"},
    {"exit", handle_exit, "Es para salir de la sesion"}};

/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */
bool builtin_is_internal(scommand cmd) {
  assert(cmd != NULL);

  // Tomo lo que el scommand tenia al frente (el ls, el wc, etc sin sus flags)
  // y armo un gstring con eso
  GString *gstr_scommand = g_string_new(scommand_front(cmd));

  // Recorre la tabla de comandos, si encuentro uno que se llame igual devuelvo
  // true
  bool b_found = false;
  for (size_t i = 0; i < CMD_COUNT && !b_found; ++i) {
    GString *gstr_command_table_name = g_string_new(command_table[i].name);
    if (g_string_equal(gstr_scommand, gstr_command_table_name)) {
      b_found = true;
    }
  }

  return b_found;
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
  // Obtengo el nombre del comando
  GString *gstr_scommand = g_string_new(scommand_front(cmd));
  // Busco el nombre en la tabla de comandos
  bool b_found = false;
  for (size_t i = 0; i < CMD_COUNT && !b_found; ++i) {
    GString *gstr_command_table_name = g_string_new(command_table[i].name);
    if (g_string_equal(gstr_scommand, gstr_command_table_name)) {
      // Ejecuta la funcion del comando con el scomando cmd
      command_table[i].handler(cmd);
      b_found = true;
    }
  }
}
