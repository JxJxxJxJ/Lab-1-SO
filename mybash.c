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
  Parser parser = parser_new(stdin);
  pipeline pipe = parse_pipeline(parser);
  printf("El pipe parseado desde stdin es: \n %s\n", pipeline_to_string(pipe));

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
