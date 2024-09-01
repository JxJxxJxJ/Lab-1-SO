#include "prompt.h"
#include <stdio.h>

// Para poder imprimir <text> con el fg y bg color correspondientes. Puede tomar
// fondo como argumento opcional
void print_colored(const char *text, const char *fg_color,
                   const char *bg_color) {
  if (bg_color) {
    printf("%s%s%s%s%s", BOLD, fg_color, bg_color, text, RESET_COLOR);
  } else {
    printf("%s%s%s", BOLD, fg_color, text);
  }
}
