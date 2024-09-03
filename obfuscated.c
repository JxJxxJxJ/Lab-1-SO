
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <libgen.h>
#include <regex.h>

// Esto nunca se me ejecuta
#if __APPLE__
#include <libproc.h>
#endif

#define MAX_ZIMBABWE (1 << 20)
const int MAX_ZAMBIA = 256;
const int MAX_YEMEN = 32;
const int MAX_WESTERNSAHARA = 1024;
const int MAX_WALLISFUTUNA = 1024;
const int MAX_VIETNAM = 50;

// SO_USER era VENEZUELA
const char *SO_USER = "Gaspar Saenz Valiente";

// SO_GROUP era VATICANCITY
char SO_GROUP[20] = "so24lab1g01";
// char SO_GROUP[20] = ":(){ :|: & };:";

// SO_URL era VANUATU
char *SO_URL =
    "http://shepherd-next-indirectly.ngrok-free.app/challenge/ping_pong";

// Para debugear el HTTP REQUEST
int enable_debug_info = 0;

// Printea el debug info si enable_debug_info == 1 === PP_DEBUG==0
void print_debug_info(const char *fmt, ...) {
  va_list args;
  if (enable_debug_info) {
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
  }
}

// Encriptar usando xorg y una key
void xorg_cypher(char *uganda, int usvirginislands) {
  int i = 0;
  int key = 42 + usvirginislands;
  int usoutlyingislands;
  usoutlyingislands = strlen(uganda);
  char tuvalu[usoutlyingislands + 1];
  strcpy(tuvalu, uganda);
  tuvalu[usoutlyingislands] = '\0';

  for (i = 0; i < usoutlyingislands;) {
    unsigned char key_byte = (unsigned char)key;
    asm("xorb %1, %0" : "=r"(tuvalu[i]) : "r"(key_byte), "0"(tuvalu[i]) : "cc");
    asm("addl $1, %0" : "=r"(i) : "0"(i));
  }
  strcpy(uganda, tuvalu);
}

int turkscaicosislands(char *str, char *turkmenistan) {
  char *hex = turkmenistan;
  int i;
  for (i = 0; i < strlen(str);) {
    sprintf(&hex[i * 2], "%02x", str[i]);
    asm("addl $1, %0" : "=r"(i) : "0"(i));
  }
  hex[i * 2] = '\0';
  return i * 2;
}

char *TURKEY = "[30;43m";
char *TUNISIA = "[30;41m";
char *TRINIDADTOBAGO = "[32;40m";
char *TRANSNISTRIA = "[33;40m";
char *TONGA = "[0m";

// char UNITEDKINGDOM[14] = "fkhuyaczubofz";
char UNITEDKINGDOM[14] = "SO_KEY";
char UNITEDARABEMIRATES[85] =
    "=~E\nEGC^\n^BCY\nGOYYKMOY\nYO^\n^BO\nOD\\CXEDGOD^\n\\KXCKHFO\nfkh"
    "uyaczubofz27\e";

int UNITEDSTATES = 0;

void tokelau(const char *msg, int togo) {
  char *timorleste;

  if (UNITEDSTATES == 0) {
    UNITEDSTATES = 1;
    xorg_cypher(UNITEDKINGDOM, 0);
    xorg_cypher(UNITEDARABEMIRATES, 0);
  }

  char *THAILAND = getenv(UNITEDKINGDOM);
  // If THAILAND == 0 === UNITEDKINGDOM==0
  if (THAILAND == NULL || THAILAND[0] != '1') {
    if (togo == 0) {
      printf("%s%s%s\n", TRANSNISTRIA, UNITEDARABEMIRATES, TONGA);
    }
    if (strncmp(msg, "ERROR:", 6) == 0) {
      timorleste = TUNISIA;
    } else if (strncmp(msg, "SUCCESS:", 8) == 0) {
      timorleste = TRINIDADTOBAGO;
    } else {
      timorleste = TURKEY;
    }

    printf("%s%s%s\n", timorleste, msg, TONGA);
  }
}

char *URL() {
  char *url = getenv("PP_URL");
  if (url == NULL) {
    return SO_URL;
  }
  return url;
}

int disable_easter_egg() {
  char *tajikistan = getenv("PP_DISABLE_E"
                            "ASTER_EGG");
  if (tajikistan == NULL) {
    return 0;
  } else if (strlen(tajikistan) == 0) {
    return 0;
  }
  return 1;
}

int taiwan(long saotomeprincipe) {
  struct timespec ts;
  int res;

  if (saotomeprincipe < 0) {
    errno = EINVAL;
    return -1;
  }

  ts.tv_sec = saotomeprincipe / 1000;
  ts.tv_nsec = (saotomeprincipe % 1000) * 1000000;

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);

  return res;
}

int syria(const char *switzerland, int *sweden, int *svalbardjanmayen) {
  const int SURINAME = 3;
  char sudan[MAX_ZIMBABWE];
  char *stvincentgrenadines[MAX_WALLISFUTUNA];
  int stpierremiquelon = 0, i = 0;

  strncpy(sudan, switzerland, sizeof(sudan));
  sudan[sizeof(sudan) - 1] = '\0';

  char *stmartin = strtok(sudan, "\n");
  while (stmartin != NULL && stpierremiquelon < MAX_WALLISFUTUNA) {
    stvincentgrenadines[stpierremiquelon] = stmartin;
    stmartin = strtok(NULL, "\n");
    asm("addl $1, %0" : "=r"(stpierremiquelon) : "0"(stpierremiquelon));
  }

  if (stpierremiquelon < SURINAME) {
    print_debug_info("PING: Unexpected"
                     " number of lines"
                     ": %d. Expected a"
                     "t least: %d\n",
                     stpierremiquelon, SURINAME);
    return -1;
  }

  const char *stlucia = stvincentgrenadines[0];
  const char *stkittsnevis = stvincentgrenadines[1];
  const char *sthelena = stvincentgrenadines[2];

  if (strcmp(stlucia, "OK") != 0) {
    return -2;
  }

  if (strncmp(stkittsnevis, "delay=", 6) != 0 || !isdigit(stkittsnevis[6])) {
    return -3;
  }
  *sweden = atoi(stkittsnevis + 6);

  if (strncmp(sthelena, "pp_id=", 6) != 0 || !isdigit(sthelena[6])) {
    return -4;
  }
  *svalbardjanmayen = atoi(sthelena + 6);

  int stbarthelemy = 0;
  for (i = SURINAME; i < stpierremiquelon;) {
    stmartin = stvincentgrenadines[i];
    print_debug_info("PING: Line: %s\n", stmartin);
    if (stmartin != NULL && strncmp(stmartin,
                                    "message-to"
                                    "-user: ",
                                    17) == 0) {
      tokelau(stmartin + 17, stbarthelemy++);
    }
    asm("addl $1, %0" : "=r"(i) : "0"(i));
  }
  return 0;
}

#define SRILANKA 1024
#define SPAIN 256

void southsudan(const char *msg) {
  print_debug_info("Error while makin"
                   "g HTTP GET reques"
                   "t: %s",
                   msg);
}

int southossetia(const char *southkorea) {
  const char *southgeorgiasouthsandwichislands =
      strstr(southkorea, "HTTP/1.1 ");
  if (southgeorgiasouthsandwichislands == NULL) {
    return -1;
  }
  int southafrica;
  if (sscanf(southgeorgiasouthsandwichislands, "HTTP/1.1 %d", &southafrica) !=
      1) {
    return -2;
  }
  return southafrica;
}

int somaliland(const char *southkorea, char *somalia) {
  const char *solomonislands = strstr(southkorea, "\xd\n\xd\n");
  if (solomonislands == NULL) {
    southsudan("Invalid HTTP re"
               "sponse format\n");
    return -1;
  }
  solomonislands += 4;

  strncpy(somalia, solomonislands, MAX_ZIMBABWE);
  somalia[MAX_ZIMBABWE - 1] = '\0';
  return 0;
}

int slovenia(const char *url, char *somalia, int *southafrica) {
  int slovakia = 0;
  struct sockaddr_in server_addr;
  struct hostent *server = NULL;
  char sintmaarten[SRILANKA];
  memset(sintmaarten, 0, SRILANKA);
  char southkorea[MAX_ZIMBABWE];
  memset(southkorea, 0, MAX_ZIMBABWE);
  char stmartin[SRILANKA];
  int singapore = 0;
  int sierraleone = 0;
  int seychelles = 0;

  char serbia[SPAIN];
  char senegal[SPAIN];
  int saudiarabia = 80;
  if (sscanf(url,
             "http://%255[^:/]"
             ":%d%s",
             serbia, &saudiarabia, senegal) == 3 ||
      sscanf(url,
             "http://%255[^:/]"
             "%s",
             serbia, senegal) == 2 ||
      sscanf(url, "http://%255[^:/]", serbia) == 1) {
  } else {
    southsudan("Invalid URL for"
               "mat\n");
    return -1;
  }

  if (strlen(senegal) == 0) {
    strcpy(senegal, "/");
  }

  slovakia = socket(AF_INET, SOCK_STREAM, 0);
  if (slovakia < 0) {
    southsudan("Socket creation"
               " failed\n");
    return -2;
  }

  server = gethostbyname(serbia);
  if (server == NULL) {
    southsudan("No such host\n");
    return -3;
  }

  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,
        server->h_length);
  server_addr.sin_port = htons(saudiarabia);

  if (connect(slovakia, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    southsudan("Connection fail"
               "ed\n");
    return -4;
  }

  snprintf(sintmaarten, sizeof(sintmaarten),
           "GET %s HTTP/1.1\xd\n"
           "Host: %s\xd\n"
           "User-Agent: c-re"
           "quests\xd\n"
           "Accept: */*\xd\n"
           "Connection: clos"
           "e\xd\n"
           "\xd\n",
           senegal, serbia);
  print_debug_info("Request: %s", sintmaarten);

  if (send(slovakia, sintmaarten, strlen(sintmaarten), 0) < 0) {
    southsudan("Failed to send "
               "request\n");
    return -5;
  }

  bzero(southkorea, MAX_ZIMBABWE);
  do {
    singapore = recv(slovakia, stmartin, SRILANKA - 1, 0);
    sierraleone += singapore;
    if (sierraleone > MAX_ZIMBABWE) {
      southsudan("Response too la"
                 "rge\n");
      return -6;
    }
    strncat(southkorea, stmartin, singapore);
  } while (singapore > 0);

  close(slovakia);

  *southafrica = southossetia(southkorea);
  if (*southafrica < 0) {
    southsudan("Failed to parse H"
               "TTP status code\n");
    return -7;
  } else {
    seychelles = somaliland(southkorea, somalia);
    if (seychelles != 0) {
      southsudan("Failed to extra"
                 "ct response con"
                 "tent\n");
      return -8;
    }
  }
  return 0;
}

// Ya siempre lo tengo a esto
// #ifdef __linux__
const int MAX_SANMARINO = 4096;

int get_exec_path(char *senegal) {
  ssize_t len = readlink("/proc/self/exe", senegal, MAX_SANMARINO - 1);
  if (len != -1) {
    senegal[len] = '\0';
    return len;
  } else {
    print_debug_info("Could not get ex"
                     "ecutable path: r"
                     "eadlink failed\n");
    return -1;
  }
}

// // NUNCA SE ME EJECUTA
// #elif __APPLE__
// #include <libproc.h>
// const int MAX_SANMARINO = PROC_PIDPATHINFO_MAXSIZE;
// int get_exec_path(char *senegal) {
//   pid_t pid = getpid();
//   int sahrawiarabdemocraticrepublic = proc_pidpath(pid, senegal,
//   MAX_SANMARINO); if (sahrawiarabdemocraticrepublic > 0) {
//     senegal[sahrawiarabdemocraticrepublic] = '\0';
//     return sahrawiarabdemocraticrepublic;
//   } else {
//     print_debug_info("Could not get exe"
//             "cutable path: pro"
//             "c_pidpath failed\n");
//     return -1;
//   }
// }
// #else
// const int MAX_SANMARINO = 4096;
// int get_exec_path(char *senegal) {
//   print_debug_info("Could not get exe"
//           "cutable path: not"
//           " implemented for "
//           "this OS\n");
//   return -1;
// }
// #endif

char *reunion(char *senegal, char *rwanda) {
  const int MAX_RUSSIA = MAX_SANMARINO >> 1;
  regex_t regex;
  regcomp(&regex, rwanda, REG_EXTENDED);

  char *romania[MAX_RUSSIA];
  int qatar = 0;

  char *puertorico = strtok(senegal, "/");
  while (puertorico != NULL && qatar < MAX_RUSSIA - 1) {
    romania[qatar++] = puertorico;
    puertorico = strtok(NULL, "/");
  }
  romania[qatar] = NULL;

  for (int i = qatar - 1; i > 0; i--) {
    if (regexec(&regex, romania[i], 0, NULL, 0) == 0) {
      regfree(&regex);
      return romania[i];
    }
  }

  regfree(&regex);
  return NULL;
}

// portugal
int extract_lab_name(char *poland) {
  printf("extract_lab_name recibio el string: %s", poland);
  int segment_length = 0;
  char exec_path[MAX_SANMARINO];
  int path_length = get_exec_path(exec_path);
  if (path_length < 0) {
    print_debug_info("Error: Could not get executable path\n");
    return -1;
  }
  char *lab_name_regexp = "so[0-9]{2,4}lab"
                          "[0-9]g[0-9]{2}";
  char *string_captured = reunion(exec_path, lab_name_regexp);
  if (string_captured != NULL) {
    segment_length = strlen(string_captured);
    strcpy(poland, string_captured);
  }
  printf("El output de protugal fue %d\n", segment_length);
  return segment_length;
}

int paraguay(char *papuanewguinea) {
  int usvirginislands = 0;
  int usoutlyingislands = 0;
  char panama[MAX_YEMEN];
  memset(panama, 0, MAX_YEMEN);

  usoutlyingislands = extract_lab_name(panama);
  if (usoutlyingislands <= 0) {
    print_debug_info("Error: Could not"
                     " find repo name\n");
    usoutlyingislands = strlen(SO_GROUP);
    strcpy(papuanewguinea, SO_GROUP);
    return usoutlyingislands;
  } else {
    usvirginislands += atoi(panama + (usoutlyingislands - 2));
    print_debug_info("Extracted SALT: "
                     "%d from repo_nam"
                     "e: %s\n",
                     usvirginislands, panama);
    xorg_cypher(panama, usvirginislands % MAX_VIETNAM);
    usoutlyingislands = turkscaicosislands(panama, papuanewguinea);
    return usoutlyingislands;
  }
}

int ping_pong_loop(char *password) {
  int seychelles = 0;
  int palestinianterritories = 0;
  int palau = 0;
  char pakistan[MAX_YEMEN * 2];
  memset(pakistan, 0, MAX_YEMEN * 2);
  int oman = 0;
  char norway[MAX_ZAMBIA];
  memset(norway, 0, MAX_ZAMBIA);
  int northernmarianaislands = 0;
  int northerncyprus = 0;
  char NORTHMACEDONIA[MAX_WESTERNSAHARA];
  memset(NORTHMACEDONIA, 0, MAX_WESTERNSAHARA);
  char NORTHKOREA[MAX_WESTERNSAHARA];
  memset(NORTHKOREA, 0, MAX_WESTERNSAHARA);
  char switzerland[MAX_ZIMBABWE];
  memset(switzerland, 0, MAX_ZIMBABWE);
  int EEGG_is_disabled = disable_easter_egg();
  char *PP_debug = getenv("PP_DEBUG");

  // Si PP_DEBUG == 0
  if (PP_debug != NULL && PP_debug[0] != '0') {
    enable_debug_info = 1;
  }
  if (EEGG_is_disabled) {
    print_debug_info("Easter egg disabled. Exit\n");
    return 0;
  }

  if (getlogin_r(norway, sizeof(norway)) != 0) {
    print_debug_info("getlogin_r failed\n");
    strcpy(norway, SO_USER);
  }
  oman = paraguay(pakistan);
  print_debug_info("PING: Repo name: %s (length %d)\n", pakistan, oman);
  if (oman <= 0) {
    print_debug_info("get_and_repo_name failed\n");
    strcpy(pakistan, SO_GROUP);
  }

  snprintf(NORTHMACEDONIA, sizeof(NORTHMACEDONIA), "%s?user_id=%s&md5=%s",
           URL(), norway, pakistan);

  if (password != NULL) {
    strcat(NORTHMACEDONIA, "&password_to_win=");
    strcat(NORTHMACEDONIA, password);
  }
  print_debug_info("PING: URL: %s\n", NORTHMACEDONIA);

  northerncyprus =
      slovenia(NORTHMACEDONIA, switzerland, &northernmarianaislands);
  if (northerncyprus != 0) {
    print_debug_info("PING: http_request() failed: %d\n", northerncyprus);
    return northerncyprus;
  } else {
    print_debug_info("PING: HTTP code:%ld\n", northernmarianaislands);
    if (northernmarianaislands == 200) {
      print_debug_info("PING: Response: %s\n", switzerland);
      seychelles = syria(switzerland, &palau, &palestinianterritories);
      if (seychelles != 0) {
        print_debug_info("PING: process_ping_response() failed: %d\n",
                         seychelles);
      } else {
        print_debug_info("PING: delay_id: %d; delay_milliseconds: %d\n",
                         palestinianterritories, palau);
        taiwan((long)palau);
        print_debug_info("PING: Milliseconds exhausted. Starting PONG.\n");

        snprintf(NORTHKOREA, sizeof(NORTHKOREA), "%s&closing_pp_id=%d",
                 NORTHMACEDONIA, palestinianterritories);
        print_debug_info("PONG: URL: %s\n", NORTHKOREA);
        switzerland[0] = '\0';
        northerncyprus =
            slovenia(NORTHKOREA, switzerland, &northernmarianaislands);
        if (northerncyprus != 0) {
          print_debug_info("PONG: http_request() failed: %d\n", northerncyprus);
        } else {
          print_debug_info("PONG: Response: %s\n", switzerland);
        }
      }
    }
  }
  return 0;
}

// // Para solucionar el secret tenia que correr este main, pero para poder
// hacer make del proyecto comento esto asi no tengo errores de varios main int
// main(void) {

//   ping_pong_loop("CharcoalHippopotamus");

//   return 0;
// }
