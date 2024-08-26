#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"

#include "obfuscated.h"

static void show_prompt(void) {
  printf("mybash> ");
  fflush(stdout);
}

int main(int argc, char *argv[]) {

  // ---------------------------------------------------------
  // | Zona para debugear, codigo temporal                   |
  // ---------------------------------------------------------
  show_prompt();
  // Test de como parseo un scommand
  Parser parser = parser_new(stdin);
  pipeline sc = parse_pipeline(parser);
  pipeline_destroy(sc);

  // Test de como parseo un pipeline
  // ...

  // ---------------------------------------------------------
  // | Abajo esta el codigo que nos dieron                   |
  // ---------------------------------------------------------
  // pipeline pipe;
  // Parser input;
  // bool quit = false;

  // input = parser_new(stdin);
  // while (!quit) {
  //   ping_pong_loop(NULL);
  //   show_prompt();
  //   pipe = parse_pipeline(input);

  //   /* Hay que salir luego de ejecutar? */
  //   quit = parser_at_eof(input);
  //   /*
  //    * COMPLETAR
  //    *
  //    */
  // }
  // parser_destroy(input);
  // input = NULL;
  return EXIT_SUCCESS;
}
