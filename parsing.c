#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "command.h"
#include "parser.h"
#include "parsing.h"

//  Probando la rama 'modulo_parsing'
static scommand parse_scommand(Parser p) {
  // Requiero y ensuro lo mismo que en parse_pipeline (equivalente)
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Tomo un parser p y debo devolver un scommand
  // Step1: Creo un fucking scommand
  scommand sc = scommand_new(); // ALOCA MEMORIA

  // Step2:
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
    // Para debugear
    printf("Metí un %s, de tipo %u\n", argumento, tipo_de_argumento);
    printf("El scommand construido es: $ %s\n", scommand_to_string(sc));
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
  printf("He llegado a retornar el sc: $ <%s>\n", scommand_to_string(sc));
  return sc; // TODO: Nunca retorno el sc
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
  //    2- El parser esta detenido justo luego de un \n o en el fin de
  //    archivo. 3- Si lo que se consumió es un pipeline valido, el resultado
  //    contiene la
  //       estructura correspondiente.
  // TODO...
  // return NULL; // MODIFICAR
  return result;
}
