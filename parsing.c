#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "command.h"
#include "parser.h"
#include "parsing.h"

static scommand parse_scommand(Parser p) {
  // Tomo un parser p y debo devolver un scommand

  // Step1: Creo un fucking scommand
  scommand sc = scommand_new(); // ALOCA MEMORIA

  // Leo los argumentos uno por uno y los pongo donde haya que ponerlos
  // en el objeto scommand
  arg_kind_t tipo_de_argumento;
  char *argumento;

  // Parseo hasta llegar a eof
  while (!parser_at_eof(p)) {
    argumento = parser_next_argument(p, &tipo_de_argumento);
    if (tipo_de_argumento == ARG_NORMAL) {
      // Lo meto como argumento normal
      scommand_push_back(sc, argumento);
      printf("Meti un %s", argumento);
    }
    if (tipo_de_argumento == ARG_INPUT) {
      // Lo meto como argumento de redir_input
      scommand_set_redir_in(sc, argumento);
      printf("Meti un %s", argumento);
    }
    if (tipo_de_argumento == ARG_OUTPUT) {
      // Lo meto como argumento de redir_output
      scommand_set_redir_out(sc, argumento);
      printf("Meti un %s", argumento);
    }
    parser_skip_blanks(p);
  }
  // Consumo el final de linea \n
  // parser_garbage === se encontraron caracteres distintos a espacios en blanco
  bool b;
  parser_garbage(p, &b);
  bool success = !b;

  if (success) {
    printf("Parseo exitoso. Se ha parseado el siguiente comando: \n %s",
           scommand_to_string(sc));
  } else { // !success
    printf(
        "Error de parseo. Se encontró el siguiente string con caracteres "
        "distintos a espacios en blanco tras correr `parser_garbage`: \n%s\n",
        parser_last_garbage(p));
  }

  // Devuelve NULL cuando hay un error de parseo
  return success ? sc : NULL;
}

/*
 * Lee todo un pipeline de `parser' hasta llegar a un fin de línea (inclusive)
 * o de archivo.
 * Devuelve un nuevo pipeline (a liberar por el llamador), o NULL en caso
 * de error.
 * REQUIRES:
 *     parser != NULL
 *     ! parser_at_eof (parser)
 * ENSURES:
 *    1- No se consumió más entrada de la necesaria
 *    2- El parser esta detenido justo luego de un \n o en el fin de archivo.
 *    3- Si lo que se consumió es un pipeline valido, el resultado contiene la
 *       estructura correspondiente.
 */
pipeline parse_pipeline(Parser p) {
  // Requieres:
  assert(p != NULL);
  assert(!parser_at_eof(p));

  pipeline result = pipeline_new();
  scommand cmd = NULL;
  bool error = false, another_pipe = true;

  cmd = parse_scommand(p);
  // Comando invalido al empezar
  error = (cmd == NULL);
  while (another_pipe && !error) {
    /*
     * COMPLETAR
     *
     */
  }
  // Opcionalmente un OP_BACKGROUND al final

  // } // <-- Esto es un error de tipeo?

  /*
   *
   * COMPLETAR
   *
   */

  // Tolerancia a espacios posteriores
  // Consumir todo lo que hay inclusive el \n
  // Si hubo error, hacemos cleanup

  // Ensures
  //    1- No se consumió más entrada de la necesaria
  //    2- El parser esta detenido justo luego de un \n o en el fin de archivo.
  //    3- Si lo que se consumió es un pipeline valido, el resultado contiene la
  //       estructura correspondiente.
  // TODO...
  // return NULL; // MODIFICAR
  return result;
}
