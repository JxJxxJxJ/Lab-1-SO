#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include <assert.h>
#include "command.h"

/*
 * ZONA DE SCOMMAND
 */

struct scommand_s
{
  GList *argumentos;
  char *redir_out;
  char *redir_in;
  size_t length;
};

/*
 * Nuevo `scommand', sin comandos o argumentos y los redirectores vacíos
 *   Returns: nuevo comando simple sin ninguna cadena y redirectores vacíos.
 * Ensures: result != NULL && scommand_is_empty (result) &&
 *  scommand_get_redir_in (result) == NULL &&
 *  scommand_get_redir_out (result) == NULL
 */
scommand scommand_new(void)
{ // NOTA: ALOCA MEMORIA
  scommand result = malloc(sizeof(*result));
  assert(result != NULL);

  result->argumentos = NULL;
  result->redir_in = NULL;
  result->redir_out = NULL;
  result->length = 0u;

  assert(result != NULL);
  assert(scommand_is_empty(result));
  assert(scommand_get_redir_in(result) == NULL);
  assert(scommand_get_redir_out(result) == NULL);
  return result;
}

/*
 * Destruye `self'.
 *   self: comando simple a destruir.
 * Requires: self != NULL
 * Ensures: result == NULL
 */
scommand scommand_destroy(scommand self)
{
  assert(self != NULL);

  g_list_free_full(self->argumentos, g_free);
  free(self->redir_in);
  free(self->redir_out);
  free(self);
  self = NULL;

  assert(self == NULL);
  return self;
}

/*
 * Agrega por detrás una cadena a la secuencia de cadenas.
 *   self: comando simple al cual agregarle la cadena.
 *   argument: cadena a agregar. El TAD se apropia de la referencia.
 * Requires: self!=NULL && argument!=NULL
 * Ensures: !scommand_is_empty(self)
 */
void scommand_push_back(scommand self, char *argument)
{
  assert(self != NULL && argument != NULL);
  self->argumentos = g_list_append(self->argumentos, argument);
  self->length++;
  assert(!scommand_is_empty(self));
}

/*
 * Quita la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual sacarle la cadena del frente.
 * Requires: self!=NULL && !scommand_is_empty(self)
 */
void scommand_pop_front(scommand self)
{
  assert(self != NULL);
  assert(!scommand_is_empty(self));
  self->argumentos = g_list_remove(self->argumentos, g_list_first(self->argumentos));
  self->length--;
}

/*
 * Define la redirección de entrada (salida).
 *   self: comando simple al cual establecer la redirección de entrada (salida).
 *   filename: cadena con el nombre del archivo de la redirección
 *     o NULL si no se quiere redirección. El TAD se apropia de la referencia.
 * Requires: self!=NULL
 */
void scommand_set_redir_in(scommand self, char *filename)
{
  assert(self != NULL);
  self->redir_in = filename;
}
void scommand_set_redir_out(scommand self, char *filename)
{
  assert(self != NULL);
  self->redir_out = filename;
}

/*
 * Indica si la secuencia de cadenas tiene longitud 0.
 *   self: comando simple a decidir si está vacío.
 *   Returns: ¿Está vacío de cadenas el comando simple?
 * Requires: self!=NULL
 */

/* Proyectores */
bool scommand_is_empty(const scommand self)
{
  assert(self != NULL);
  return self->length == 0u;
}

/*
 * Da la longitud de la secuencia cadenas que contiene el comando simple.
 *   self: comando simple a medir.
 *   Returns: largo del comando simple.
 * Requires: self!=NULL
 * Ensures: (scommand_length(self)==0) == scommand_is_empty(self)
 *
 */
unsigned int scommand_length(const scommand self)
{
  assert(self != NULL);
  assert(scommand_length(self)==0) == scommand_is_empty(self)));
  return self->length;
}

/*
 * Toma la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual tomarle la cadena del frente.
 *   Returns: cadena del frente. La cadena retornada sigue siendo propiedad
 *     del TAD, y debería considerarse inválida si luego se llaman a
 *     modificadores del TAD. Hacer una copia si se necesita una cadena propia.
 * Requires: self!=NULL && !scommand_is_empty(self)
 * Ensures: result!=NULL
 */
char *scommand_front(const scommand self)
{
  assert(self != NULL);
  assert(!scommand_is_empty(self));

  char *result = g_list_first(self->argumentos);

  assert(result != NULL);
  return result;
}

/*
 * Obtiene los nombres de archivos a donde redirigir la entrada (salida).
 *   self: comando simple a decidir si está vacío.
 *   Returns: nombre del archivo a donde redirigir la entrada (salida)
 *  o NULL si no está redirigida.
 * Requires: self!=NULL
 */
char *scommand_get_redir_in(const scommand self)
{
  assert(self!=NULL);
  return self->redir_in;
}

char *scommand_get_redir_out(const scommand self)
{
  assert(self!=NULL);
  return self->redir_out;
}

/* Preety printer para hacer debugging/logging.
 * Genera una representación del comando simple en un string (aka "serializar")
 *   self: comando simple a convertir.
 *   Returns: un string con la representación del comando simple similar
 *     a lo que se escribe en un shell. El llamador es dueño del string
 *     resultante.
 * Requires: self!=NULL
 * Ensures: scommand_is_empty(self) ||
 *   scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL ||
 *   strlen(result)>0
 */
char *scommand_to_string(const scommand self){
  assert(self!=NULL));
  size_t bytes_necesarios_para_alocar = 0u;

  char* result = malloc(sizeof(bytes_necesarios_para_alocar))

  assert(scommand_is_empty(self) ||
  scommand_get_redir_in(self)==NULL ||
  scommand_get_redir_out(self)==NULL) ||
  strlen(result) > 0)
}

/*
 * ZONA DE PIPELINE
 */

struct pipeline_s
{
  GList *scomandos;
  bool esta_en_primer_plano;
};

/*
 * Nuevo `pipeline', sin comandos simples y establecido para que espere.
 *   Returns: nuevo pipeline sin comandos simples y que espera.
 * Ensures: result != NULL
 *  && pipeline_is_empty(result)
 *  && pipeline_get_wait(result)
 */
pipeline pipeline_new(void)
{
  pipeline result = malloc(sizeof(*result));
  result->scomandos = NULL;
  result->esta_en_primer_plano = false;
  return result;
}
