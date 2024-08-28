#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "command.h"
#include "parser.h"
#include "parsing.h"

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
  // Precondiciones.
  assert(p != NULL);
  assert(!parser_at_eof(p));

  // Inicialización de variables.
  pipeline result = pipeline_new();
  scommand cmd = NULL;
  bool error = false, another_pipe = true;

  cmd = parse_scommand(p); 
  error = (cmd == NULL); // Comando invalido al empezar.
  
  while (another_pipe && !error) {
    // Agrego el comando a la secuencia de la pipeline.
    pipeline_push_back(result,cmd);   

    // Verifico si existe una '|' en la entrada.
    parser_op_pipe(p,&another_pipe);

    // En caso de existir una '|', seteo cmd y error para parsear el siguiente comando simple. Si no, salgo del bucle.
    if(another_pipe) {
      cmd = parse_scommand(p);  // ¿Quizás sea necesario un 'parser_skip_blanks()' para saltar al siguiente comando?
      error = (cmd == NULL)
    }

    // Usando como ejemplo ls -l -a < entrada.txt > salida.txt | wc -l
    // 1° iteración: result = ( [ (["ls","-l","-a"], salida.txt , entrada.txt) ], ... )
    // 2° iteración: result = ( [ (["ls","-l","-a"], salida.txt , entrada.txt) , (["wc","-l"], NULL , NULL) ], ... )
  }

  // Opcionalmente un OP_BACKGROUND al final.
  // Verifica si la pipeline se ejecuta en primer o segundo plano.
  bool in_background;
  parser_op_background(p,&in_background);
  pipeline_set_wait(result,!in_background);  

  // result = ( [ (["ls","-l","-a"], salida.txt , entrada.txt) , (["wc","-l"], NULL , NULL) ], true )

  // Tolerancia a espacios posteriores.
  parser_skip_blanks(p);

  // Consumir todo lo que hay inclusive el \n.
  bool trash;
  parser_garbage(p,&trash);

  // Si hubo error, hacemos cleanup.
  if(error) {
    result = pipeline_destroy(result);
    result = NULL;
  }

  return result;
}
