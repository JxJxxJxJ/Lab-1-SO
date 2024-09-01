#ifndef PROMPT_H
#define PROMPT_H
#include <stdio.h>

#define CYANBG "\033[48;5;80m"      // Background color -> RGB 50CACD
#define CYANFG "\033[38;5;80m"      // Foreground color -> RGB 50CACD
#define AMARILLOBG "\033[48;5;180m" // Background color -> RGB E0AF85
#define AMARILLOFG "\033[38;5;180m" // Foreground color -> RGB E0AF85
#define VERDEBG "\033[48;5;114m"    // Background color -> RGB 80C990
#define VERDEFG "\033[38;5;114m"    // Foreground color -> RGB 80C990
#define VERDE2BG "\033[48;5;42m"    //
#define VERDE2FG "\033[38;5;42m"    //
#define CELESTEBG "\033[48;5;116m"  // Background color -> RGB 74C3E4
#define CELESTEFG "\033[38;5;116m"  // Foreground color -> RGB 74C3E4
#define AZULBG "\033[48;5;75m"      //
#define AZULFG "\033[38;5;75m"      //
#define LILABG "\033[48;5;182m"     // Background color -> RGB E6A3DC
#define LILAFG "\033[38;5;182m"     // Foreground color -> RGB E6A3DC
#define SALMONBG "\033[48;5;217m"   // Background color -> RGB F2A1C2
#define SALMONFG "\033[38;5;217m"   // Foreground color -> RGB F2A1C2
#define MORADOBG "\033[48;5;147m"   // Background color -> RGB A3B8EF
#define MORADOFG "\033[38;5;147m"   // Foreground color -> RGB A3B8EF
#define NEGROBG "\033[48;5;16m"     // Background color -> RGB 101010
#define NEGROFG "\033[38;5;16m"     // Foreground color -> RGB 101010
#define NEGRO2FG "\033[38;5;235m"   //
#define NEGRO2BG "\033[48;5;235m"   //
#define GRISBG "\033[48;5;145m"     // Background color -> RGB C0C0C0
#define GRISFG "\033[38;5;145m"     // Foreground color -> RGB C0C0C0
#define BLANCOFG "\033[38;5;255m"   //
#define BLANCOBG "\033[48;5;255m"   //
#define RESET_COLOR "\033[0m"       // Texto default
#define BOLD "\033[1m"              // Texto boldo

// Para poder imprimir <text> con el fg y bg color correspondientes
void print_colored(const char *text, const char *fg_color,
                   const char *bg_color);

#endif
