#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <stdbool.h>

#include "command.h"

bool builtin_is_internal(scommand cmd);
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */

bool builtin_alone(pipeline p);
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

void builtin_run(scommand cmd);
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */

// ╔════════════════════════════════════════════════════════════════════╗
// ║ Todo esto que agrego es para que ande el poder extender el builtin ║
// ╚════════════════════════════════════════════════════════════════════╝

// Enum de los comandos que myBash acepta
// CMD_COUNT es la cantidad de comandos aceptados hasta ahora por la shell
typedef enum {
  CMD_CD,
  CMD_HELP,
  CMD_EXIT,
  CMD_carita,
  CMD_cmds,
  CMD_COUNT
} Command;

// Cada comando <comando> tendrá una función void asociada <handle_comando> y
// una descripción de lo que hace en help

// Uso const para decir que los datos son solo de lectura
/* void * (*function)();
 * “declare a function pointer named function, which points to a function that
 * takes an unspecified number of arguments, then returns a void *"
 *
 * <tipo_de_output> (*<nombre_de_funcion>) (<tipo_de_argumento_de_la_funcion>)
 */

// Todo comando deberá tener una función que se encargue de ejecutar la tarea
// del comando y una descripción obtenible al hacer <comando> --help
typedef struct {
  void (*handler)(scommand); // Función que maneja el comando
  const char *help;          // Descripción del comando
} CMD_DATA;                  
                             
typedef struct {             
  const char *name;          // Nombre del comando
  const CMD_DATA data;       // Data del comando
} CMD_ENTRY;

// Un arreglo global de CMD_ENTRY.
// Este arreglo lleva el registro de todos los comandos builtin de myBash
// Para agregar un comando al myBash, debo agregar una entrada al arreglo y
// proveer un puntero a una función handle válida
// extern le dice al preprocesador que el arreglo existe pero que otro lo va a
// definir (builtin.c). Es para poder usarlo "desde cualquier lado" en builtin.c.
extern CMD_ENTRY commands_registry[CMD_COUNT];

#endif
