#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "command.h"
#include "parser.h"
#include "parsing.h"

// TEMPORAL
/* static */ scommand parse_scommand(Parser p) {
  // Requiero y ensuro lo mismo que en parse_pipeline (equivalente)
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Tomo un parser p y debo devolver un scommand
  scommand sc = scommand_new(); // ALOCA MEMORIA

  // Leo los argumentos uno por uno y los pongo donde haya que ponerlos
  // en el objeto scommand
  arg_kind_t tipo_de_argumento;
  char *argumento = NULL;

  // Parseo hasta el final del archivo
  while (!parser_at_eof(p)) { // TODO: Por alguna razon nunca se llega al eof
    argumento = parser_next_argument(p, &tipo_de_argumento);
    if (argumento == NULL) {
      break;
    }
    if (argumento != NULL) {
      switch (tipo_de_argumento) {
      case ARG_NORMAL:
        scommand_push_back(sc, argumento);
        break;
      case ARG_INPUT:
        scommand_set_redir_in(sc, argumento);
        break;
      case ARG_OUTPUT:
        scommand_set_redir_out(sc, argumento);
        break;
      }
    }
    parser_skip_blanks(p);
  }
  // Ensuro lo siguiente
  //  1- No se consumió más entrada de la necesaria
  // assert(???);
  //  2- El parser esta detenido justo luego de un \n o en el fin de archivo.
  // assert(parser_at_eof(p));
  //  3- Si lo que se consumió es un scommand valido, el resultado contiene la
  //     estructura correspondiente.
  // assert(???);
  return sc;
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
  // Precondiciones.
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Inicialización de variables.
  pipeline result = pipeline_new();
  scommand cmd = NULL;
  bool error = false, another_pipe = true;
  cmd = parse_scommand(p);
  error = (cmd == NULL); // Comando invalido al empezar.
  if (another_pipe && error) {
    printf("Error: no input or output file.\n");
    pipeline_destroy(result);
    return NULL;
  }
  while (another_pipe && !error) {
    pipeline_push_back(result, cmd);
    parser_op_pipe(p, &another_pipe);
    if (another_pipe) {
      cmd = parse_scommand(p);
      error = (cmd == NULL);
    }
  }
  bool in_background;
  parser_op_background(p, &in_background);
  pipeline_set_wait(result, !in_background);
  bool trash;
  parser_garbage(p, &trash);

  if (error) {
    result = pipeline_destroy(result);
    result = NULL;
  }

  // ...
  if (pipeline_length(result) == 1 &&
      scommand_length(pipeline_front(result)) == 0) {
    pipeline_destroy(result);
    result = NULL;
  }

  return result;
}
