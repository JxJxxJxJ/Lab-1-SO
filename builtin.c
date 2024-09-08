#include "builtin.h"
#include "command.h"
#include "tests/syscall_mock.h"
#include <assert.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir

static void handle_carita(scommand sc) { printf(":)\n"); }


static void handle_cd(scommand sc) {
  // Planteo dos casos: uno en el que solo se coloca el cd sin dirección y
  // otro donde se le da una dirección específica

  // Si el tamaño del scommand es 1, significa que se trata del cd solo sin
  // dirección
  if (scommand_length(sc) == 1) {
    chdir(getenv("HOME")); // La variable 'HOME' está configurada para apuntar
                           // al directorio personal del usuario actual,
                           // entonces getenv accede al valor de dicha
                           // variable y me transporta a esa dirección
  } else { // Si el tamaño del scommand es != 1, significa que el cd viene
           // acompañado de una dirección
    scommand_pop_front(
        sc); // Popeo el cd para quedarme solamente con la dirección
    char *path =
        scommand_to_string(sc); // Guardo la dirección en la variable path
    int k = chdir(path); // Intento llegar a la dirección con chdir (caso
                         // exitoso cuando k == 0)

    // Si k == -1, entonces no se puede acceder a la ruta entregada
    if (k == -1) {
      printf("myBash: cd: %s: El archivo o directorio no existe.\n", path);
    }
  }
}


static void handle_exit(scommand sc) {
  printf("¡Adiós!\n");
  exit(EXIT_SUCCESS);
}


static void handle_cmds(scommand sc) {
  printf("Los comandos internos de este bash son:\n");

  // Busco la length del comando más largo
  size_t max_length = 0;
  for (size_t i = 0; i < CMD_COUNT; i++) {
    size_t name_length = strlen(commands_registry[i].name);
    if (name_length > max_length) {
      max_length = name_length;
    }
  }
  for (size_t i = 0; i < CMD_COUNT; i++) {
    // %*s significa que el length del string lo doy como un argumento
    // %-s left-alineo el string, podría pero a mí no me gusta
    printf("%*s : %s\n", (int)max_length, commands_registry[i].name,
           commands_registry[i].data.help);
  }
}


static void handle_help(scommand sc) {
  printf("  ███╗   ███╗██╗   ██╗██████╗  █████╗ ███████╗██╗  ██╗    ██╗   ██╗ ██╗    ██████╗  \n");
  printf("  ████╗ ████║╚██╗ ██╔╝██╔══██╗██╔══██╗██╔════╝██║  ██║    ██║   ██║███║   ██╔═████╗ \n");
  printf("  ██╔████╔██║ ╚████╔╝ ██████╔╝███████║███████╗███████║    ██║   ██║╚██║   ██║██╔██║ \n");
  printf("  ██║╚██╔╝██║  ╚██╔╝  ██╔══██╗██╔══██║╚════██║██╔══██║    ╚██╗ ██╔╝ ██║   ████╔╝██║ \n");
  printf("  ██║ ╚═╝ ██║   ██║   ██████╔╝██║  ██║███████║██║  ██║     ╚████╔╝  ██║██╗╚██████╔╝ \n");
  printf("  ╚═╝     ╚═╝   ╚═╝   ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝      ╚═══╝   ╚═╝╚═╝ ╚═════╝  \n");
  printf("╔═══════════════════════════════════════════════════════════════════════════════════╗ \n");
  printf("║            Está usando la shell codificada por :(){ :|:& };: (grupo 1)            ║ \n");
  printf("╚═══════════════════════════════════════════════════════════════════════════════════╝ \n\n");          
  printf("Los autores que colaboraron para la creación del mismo son: \n- Juan "
         "Cruz Hermosilla Artico\n- Gaspar Saenz Valiente\n- Exequiel "
         "Trinidad\n- Fernando Cabrera Luque\n\n");
  // Printeo ademas los comandos
  handle_cmds(sc);
}

// Un arreglo global de CMD_ENTRY
// Este arreglo lleva el registro de todos los comandos builtin de myBash
// Para agregar un comando al myBash, debo agregar una entrada al arreglo y
// proveer un puntero a una funcion handle válida
CMD_ENTRY commands_registry[CMD_COUNT] = {
    {"cd",
     {handle_cd, "Permite desplazarse entre los directorios del sistema."}},
    {"help", {handle_help, "Muestra información de myBash."}},
    {"exit", {handle_exit, "Es para salir de la sesión."}},
    {":)", {handle_carita, "Printea una carita."}},
    {"cmds",
     {handle_cmds, "Muestra los comandos builtin y sus descripciones."}}};

// Devuelva un puntero a una GQueue populado con los nombres
// gstringuificados de CMD_ENTRY commands_registry
static GQueue *init_gq_command_table(void) {
  GQueue *gq_command_table = g_queue_new();
  // Tomo los nombres de los comandos en commands_registry, los gstringuifico
  // y los meto a la GQueue* gq_command_table
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
// | los comandos que acepta myBash                                       |
// ------------------------------------------------------------------------

bool builtin_is_internal(scommand cmd) {
  assert(cmd != NULL);
  bool found = false;
  // Inicializo la gq_command_table para realizar la busqueda ALOCA MEMORIA
  GQueue *gq_command_table = init_gq_command_table();

  // Tomo lo que el scommand cmd tenía al frente (el cd, el help, etc. sin sus
  // flags) y armo un string con eso
  char *str_scommand = scommand_front(cmd);

  // Busco manualmente en la GQueue el str_scommand
  for (unsigned int i = 0; i < g_queue_get_length(gq_command_table) && !found;
       i++) {
    char *name_pointer = (char *)g_queue_peek_nth(gq_command_table, i);
    if (strcmp(name_pointer, str_scommand) == 0) {
      found = true;
    }
  }

  // Destruyo la gq_command_table con la función para liberar cada elemento
  // (son strings, los libero con free)
  g_queue_free_full(gq_command_table, free);
  return found;
}
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */


bool builtin_alone(pipeline p) {
  assert(p != NULL);
  return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
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


void builtin_run(scommand cmd) {
  assert(builtin_is_internal(cmd));
  // Inicializo la gq_command_table para realizar la busqueda (ALOCA MEMORIA)
  GQueue *gq_command_table = init_gq_command_table();

  // Obtengo el nombre del comando
  char *str_scommand = scommand_front(cmd);

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
    // Ejecuta la función del comando con el scommand cmd
    commands_registry[indx].data.handler(cmd);
  }

  g_queue_free_full(gq_command_table, free);
}
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */
