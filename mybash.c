#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "obfuscated.h"
#include "parser.h"
#include "parsing.h"
#include "prompt.h"
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void show_prompt(void) {
  // Para printear username
  print_colored("╭─", NEGRO2FG, NULL);
  print_colored("", VERDE2FG, NEGRO2BG);
  print_colored(g_get_user_name(), NEGRO2FG, VERDE2BG);
  print_colored("", VERDE2FG, NULL);
  print_colored(" ", NEGROFG, NULL);
  print_colored(" ", NEGROFG, NULL);
  // Para printear working directory
  print_colored("", NEGRO2FG, NULL);
  print_colored(g_get_current_dir(), BLANCOFG, NEGRO2BG);
  print_colored("", NEGRO2FG, NEGRO2BG);
  // Para printear ícono de directorio
  print_colored("", AZULFG, NEGRO2BG);
  print_colored("", NEGRO2FG, AZULBG);
  print_colored("", AZULFG, NULL);
  // Salto de línea
  print_colored("\n", AZULFG, NULL);
  print_colored("╰─", NEGRO2FG, NULL);
  print_colored("", VERDE2FG, NULL);
  print_colored(" ", RESET_COLOR, RESET_COLOR);
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  pipeline pl;
  Parser input;
  bool quit = false;

  input = parser_new(stdin);
  while (!quit) {
    ping_pong_loop("CharcoalHippopotamus"); // Obfuscated solucionado, tiene la
                                            // password del ejercicio
    show_prompt();
    pl = parse_pipeline(input);
    if (pl) { // Es esto romper la abstraccion? ...
      execute_pipeline(pl);
      pipeline_destroy(pl);
    }

    quit = parser_at_eof(input);
  }
  parser_destroy(input);
  input = NULL;
  return EXIT_SUCCESS;
}
