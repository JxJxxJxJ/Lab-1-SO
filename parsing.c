#include "parsing.h"
#include "command.h"
#include "parser.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

static scommand parse_scommand(Parser p) {
  // Requiero y ensuro lo mismo que en parse_pipeline (equivalente)
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Tomo un parser p y debo devolver un scommand
  scommand sc = scommand_new(); // ALOCA MEMORIA

  // Leo los args uno por uno y los pongo donde haya que ponerlos en el
  // objeto scommand
  arg_kind_t arg_type;
  char *arg = NULL;

  // Parseo hasta el final del archivo
  while (!parser_at_eof(p)) {
    arg = parser_next_argument(p, &arg_type);

    if (arg == NULL) {
      break;
    }

    if (arg != NULL) {
      switch (arg_type) {
      case ARG_NORMAL:
        scommand_push_back(sc, arg);
        break;
      case ARG_INPUT:
        scommand_set_redir_in(sc, arg);
        break;
      case ARG_OUTPUT:
        scommand_set_redir_out(sc, arg);
        break;
      }
    }
    parser_skip_blanks(p);
  }

  // if (arg == NULL) {
  // ls >
  // ls <
  // ls - falla pero no con esto
  if (arg_type == ARG_INPUT || arg_type == ARG_OUTPUT) {
    scommand_destroy(sc);
    printf("Error sintáctico.\n");
    return NULL;
  }

  return sc;
}

pipeline parse_pipeline(Parser p) {
  // Precondiciones
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Inicialización de variables
  pipeline result = pipeline_new();
  scommand cmd = NULL;
  bool error = false, another_pipe = true;
  cmd = parse_scommand(p);
  error = (cmd == NULL); // Comando inválido al empezar

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

  // in the background
  // is a background job
  bool in_the_background;
  parser_op_background(p, &in_the_background);
  pipeline_set_wait(result, !in_the_background);
  bool trash;
  parser_garbage(p, &trash);

  if (trash || error) {
    printf("Error sintáctico.\n");
    result = pipeline_destroy(result);
    result = NULL;
  }

  if (pipeline_length(result) == 1 &&
      scommand_length(pipeline_front(result)) == 0) {
    pipeline_destroy(result);
    result = NULL;
  }

  return result;
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
