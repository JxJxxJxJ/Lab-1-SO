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
  // Obtén el nombre del usuario y el nombre del host usando GLib
  // const gchar *usr = g_get_user_name();
  // const gchar *machine = g_get_host_name();

  // Para printear username
  print_colored("╭─", NEGRO2FG, NULL);
  print_colored("", VERDE2FG, NEGRO2BG);
  print_colored(g_get_user_name(), NEGROFG, VERDE2BG);
  print_colored("", VERDE2FG, NULL);
  print_colored(" ", NEGROFG, NULL);
  print_colored(" ", NEGROFG, NULL);
  // Para printear working directory
  print_colored("", NEGRO2FG, NULL);
  print_colored(g_get_current_dir(), BLANCOFG, NEGRO2BG);
  print_colored("", NEGRO2FG, NEGRO2BG);
  // Para printear icono de directorio
  print_colored("", AZULFG, NULL);
  print_colored("", NEGROFG, AZULBG);
  print_colored("", AZULFG, NULL);
  // Salto de linea
  print_colored("\n", AZULFG, NULL);
  print_colored("╰─", NEGRO2FG, NULL);
  print_colored("", VERDE2FG, NULL);
  print_colored(" ", RESET_COLOR, RESET_COLOR);
  fflush(stdout);
}

int main(int argc, char *argv[]) {

  // ---------------------------------------------------------
  // | Zona para debugear builtin codigo temporal                   |
  // ---------------------------------------------------------
  while (true) {
    show_prompt();
    // Test de como parseo un scommand
    Parser parser = parser_new(stdin);
    scommand sc = parse_scommand(parser);
    builtin_run(sc);
    char *currentDirectory = (char *)g_get_current_dir();
    printf("%s\n", currentDirectory);
    scommand_destroy(sc);
  }
  // ---------------------------------------------------------
  // | Zona para debugear, codigo temporal                   |
  // ---------------------------------------------------------
  // show_prompt();
  // // Test de como parseo un scommand
  // Parser parser = parser_new(stdin);
  // pipeline pl = parse_pipeline(parser);
  // pipeline_destroy(pl);

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
