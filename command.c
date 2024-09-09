#include "command.h"
#include <assert.h>
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ZONA DE SCOMMAND
 */

struct scommand_s {
  GQueue *args;
  char *redir_in;
  char *redir_out;
};

scommand scommand_new(void) { // NOTA: ALOCA MEMORIA
  scommand result = malloc(sizeof(*result));
  assert(result != NULL);
  result->args = g_queue_new();
  result->redir_in = NULL;
  result->redir_out = NULL;
  assert(result != NULL);
  assert(scommand_is_empty(result));
  assert(scommand_get_redir_in(result) == NULL);
  assert(scommand_get_redir_out(result) == NULL);
  return result;
}
/*
 * Nuevo `scommand', sin comandos o argumentos y los redirectores vacíos
 *   Returns: nuevo comando simple sin ninguna cadena y redirectores vacíos.
 * Ensures: result != NULL && scommand_is_empty (result) &&
 *  scommand_get_redir_in (result) == NULL &&
 *  scommand_get_redir_out (result) == NULL
 */

scommand scommand_destroy(scommand self) {
  assert(self != NULL);
  g_queue_free_full(self->args, g_free);
  self->args = NULL;
  free(self->redir_in);
  free(self->redir_out);
  free(self);
  self = NULL;
  assert(self == NULL);
  return self;
}
/*
 * Destruye `self'.
 *   self: comando simple a destruir.
 * Requires: self != NULL
 * Ensures: result == NULL
 */

void scommand_push_back(scommand self, char *argument) {
  assert(self != NULL);
  assert(argument != NULL);
  g_queue_push_tail(self->args, argument);
  assert(!scommand_is_empty(self));
}
/*
 * Agrega por detrás una cadena a la secuencia de cadenas.
 *   self: comando simple al cual agregarle la cadena.
 *   argument: cadena a agregar. El TAD se apropia de la referencia.
 * Requires: self!=NULL && argument!=NULL
 * Ensures: !scommand_is_empty(self)
 */

void scommand_pop_front(scommand self) {
  assert(self != NULL && !scommand_is_empty(self));
  // Obtengo el elemento del front
  char *front_item = g_queue_peek_head(self->args);
  // Lo saco de la gqueue
  gboolean b = g_queue_remove(self->args, g_queue_peek_head(self->args));
  // Lo libero
  free(front_item);
  assert(b);
}
/*
 * Quita la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual sacarle la cadena del frente.
 * Requires: self!=NULL && !scommand_is_empty(self)
 */

void scommand_set_redir_in(scommand self, char *filename) {
  assert(self != NULL);
  // Libero por las dudas, porque si ya había algo entonces no lo estoy
  // liberando
  free(self->redir_in);
  self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char *filename) {
  assert(self != NULL);
  // Libero por las dudas, porque si ya había algo entonces no lo estoy
  // liberando
  free(self->redir_out);
  self->redir_out = filename;
}
/*
 * Define la redirección de entrada (salida).
 *   self: comando simple al cual establecer la redirección de entrada (salida).
 *   filename: cadena con el nombre del archivo de la redirección
 *     o NULL si no se quiere redirección. El TAD se apropia de la referencia.
 * Requires: self!=NULL
 */

/* Proyectores */

bool scommand_is_empty(const scommand self) {
  assert(self != NULL);
  return g_queue_get_length(self->args) == 0u;
}
/*
 * Indica si la secuencia de cadenas tiene longitud 0.
 *   self: comando simple a decidir si está vacío.
 *   Returns: ¿Está vacío de cadenas el comando simple?
 * Requires: self!=NULL
 */

unsigned int scommand_length(const scommand self) {
  assert(self != NULL);
  // Uso self->length en vez de scommand_length(self) para evitar infinita
  // recursión
  assert((g_queue_get_length(self->args) == 0) == scommand_is_empty(self));
  return g_queue_get_length(self->args);
}
/*
 * Da la longitud de la secuencia cadenas que contiene el comando simple.
 *   self: comando simple a medir.
 *   Returns: largo del comando simple.
 * Requires: self!=NULL
 * Ensures: (scommand_length(self)==0) == scommand_is_empty(self)
 *
 */

char *scommand_front(const scommand self) {
  assert(self != NULL);
  assert(!scommand_is_empty(self));
  char *result = g_queue_peek_head(self->args);
  assert(result != NULL);
  return result;
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

char *scommand_get_redir_in(const scommand self) {
  assert(self != NULL);
  return self->redir_in;
}
char *scommand_get_redir_out(const scommand self) {
  assert(self != NULL);
  return self->redir_out;
}
/*
 * Obtiene los nombres de archivos a donde redirigir la entrada (salida).
 *   self: comando simple a decidir si está vacío.
 *   Returns: nombre del archivo a donde redirigir la entrada (salida)
 *  o NULL si no está redirigida.
 * Requires: self!=NULL
 */

char *scommand_to_string(const scommand self) {
  assert(self != NULL);
  // Creo un GString vacío
  GString *gstr = g_string_new(NULL);
  // Itero sobre self->args y voy agregando los comandos al string sin romper la
  // abstracción
  for (uint i = 0; i < g_queue_get_length(self->args); i++) {
    gstr = g_string_append(gstr, g_queue_peek_nth(self->args, i));
    if (i != g_queue_get_length(self->args) - 1) {
      gstr = g_string_append_c(gstr, ' ');
    }
  }
  // Le meto las redirecciones si existen
  if (self->redir_in != NULL) {
    gstr = g_string_append(gstr, " < ");
    gstr = g_string_append(gstr, self->redir_in);
  }
  if (self->redir_out != NULL) {
    gstr = g_string_append(gstr, " > ");
    gstr = g_string_append(gstr, self->redir_out);
  }
  // Libera la memoria de la estructura del GString gstr y me devuelve la data
  // adentro como char*
  char *result = g_string_free_and_steal(gstr);
  assert(scommand_is_empty(self) || scommand_get_redir_in(self) == NULL ||
         scommand_get_redir_out(self) == NULL || strlen(result) > 0);
  return result;
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

/*
 * ZONA DE PIPELINE
 */

struct pipeline_s {
  GQueue *scommands;
  bool is_in_foreground;
  size_t length;
};

pipeline pipeline_new(void) {
  pipeline result = malloc(sizeof(*result));
  result->scommands = g_queue_new();
  result->is_in_foreground = true;
  result->length = 0u;
  return result;
}
/*
 * Nuevo `pipeline', sin comandos simples y establecido para que espere.
 *   Returns: nuevo pipeline sin comandos simples y que espera.
 * Ensures: result != NULL
 *  && pipeline_is_empty(result)
 *  && pipeline_get_wait(result)
 */

// Wrapper para castear scommand_destroy a GDestroyNotify
static void scommand_destroy_wrapper(void *data) {
  scommand_destroy((scommand)data);
}
// NOTA: Si a alguien se le ocurre otra forma sin romper la abstracción acá,
// cambielo
pipeline pipeline_destroy(pipeline self) {
  assert(self != NULL);
  g_queue_free_full(self->scommands, (GDestroyNotify)scommand_destroy_wrapper);
  free(self);
  return NULL;
}
/*
 * Destruye `self'.
 *   self: tubería a destruir.
 * Requires: self != NULL
 * Ensures: result == NULL <-- No deberia ser self == NULL?
 */

/* Modificadores */

void pipeline_push_back(pipeline self, scommand sc) {
  assert(self != NULL);
  assert(sc != NULL);
  g_queue_push_tail(self->scommands, sc);
  self->length++;
  assert(!pipeline_is_empty(self));
}
/*
 * Agrega por detrás un comando simple a la secuencia.
 *   self: pipeline al cual agregarle el comando simple.
 *   sc: comando simple a agregar. El TAD se apropia del comando.
 * Requires: self!=NULL && sc!=NULL
 * Ensures: !pipeline_is_empty(self)
 */

void pipeline_pop_front(pipeline self) {
  assert(self != NULL);
  assert(!pipeline_is_empty(self));
  // g_queue_pop_head popea el primer elemento y lo devuelve como data
  scommand killme = g_queue_pop_head(self->scommands);
  scommand_destroy(killme);
  self->length--;
}
/*
 * Quita el comando simple de adelante de la secuencia.
 *   self: pipeline al cual sacarle el comando simple del frente.
 *      Destruye el comando extraido.
 * Requires: self!=NULL && !pipeline_is_empty(self)
 */

void pipeline_set_wait(pipeline self, const bool w) {
  assert(self != NULL);
  self->is_in_foreground = w;
}
/*
 * Define si el pipeline tiene que esperar o no.
 *   self: pipeline que quiere ser establecido en su atributo de espera.
 * Requires: self!=NULL
 */

/* Proyectores */

bool pipeline_is_empty(const pipeline self) {
  assert(self != NULL);
  return self->length == 0u;
}
/*
 * Indica si la secuencia de comandos simples tiene longitud 0.
 *   self: pipeline a decidir si está vacío.
 *   Returns: ¿Está vacío de comandos simples el pipeline?
 * Requires: self!=NULL
 */

unsigned int pipeline_length(const pipeline self) {
  assert(self != NULL);
  // Uso self->length en vez de pipeline_length para evitar recursión infinita
  assert((self->length == 0) == pipeline_is_empty(self));
  return self->length;
}
/*
 * Da la longitud de la secuencia de comandos simples.
 *   self: pipeline a medir.
 *   Returns: largo del pipeline.
 * Requires: self!=NULL
 * Ensures: (pipeline_length(self)==0) == pipeline_is_empty(self)
 *
 */

scommand pipeline_front(const pipeline self) {
  assert(self != NULL);
  assert(!pipeline_is_empty(self));
  scommand result = g_queue_peek_head(self->scommands);
  assert(result != NULL);
  return result;
}
/*
 * Devuelve el comando simple de adelante de la secuencia.
 *   self: pipeline al cual consultar cual es el comando simple del frente.
 *   Returns: comando simple del frente. El comando devuelto sigue siendo
 *      propiedad del TAD.
 *      El resultado no es un "const scommand" ya que el llamador puede
 *      hacer modificaciones en el comando, siempre y cuando no lo destruya.
 * Requires: self!=NULL && !pipeline_is_empty(self)
 * Ensures: result!=NULL
 */

bool pipeline_get_wait(const pipeline self) {
  assert(self != NULL);
  return self->is_in_foreground;
}
/*
 * Consulta si el pipeline tiene que esperar o no.
 *   self: pipeline a decidir si hay que esperar.
 *   Returns: ¿Hay que esperar en el pipeline self?
 * Requires: self!=NULL
 */

char *pipeline_to_string(const pipeline self) {
  assert(self != NULL);
  // Creo un GString vacío
  GString *gstr = g_string_new(NULL);
  // Itero sobre self->scommands y voy agregando los comandos al string sin
  // romper la abstracción
  for (guint i = 0; i < g_queue_get_length(self->scommands); i++) {
    // Añado la representación del scommand en formato de string
    gstr = g_string_append(
        gstr, scommand_to_string(g_queue_peek_nth(self->scommands, i)));
    if (i != g_queue_get_length(self->scommands) - 1) {
      gstr = g_string_append(gstr, " | ");
    }
  }
  // Le meto un & si no hay que esperar a que la pipeline termine (segundo
  // plano) y que al menos haya un comando por correr
  if (!(self->is_in_foreground) && g_queue_get_length(self->scommands) >= 1) {
    gstr = g_string_append(gstr, " &");
  }
  // Libera la memoria de la estructura del GString gstr y me devuelve la data
  // adentro como char*
  char *result = g_string_free_and_steal(gstr);
  assert(pipeline_is_empty(self) || pipeline_get_wait(self) ||
         strlen(result) > 0);
  return result;
}
/* Pretty printer para hacer debugging/logging.
 * Genera una representación del pipeline en una cadena (aka "serializar").
 *   self: pipeline a convertir.
 *   Returns: una cadena con la representación del pipeline similar
 *     a lo que se escribe en un shell. Debe destruirla el llamador.
 * Requires: self!=NULL
 * Ensures: pipeline_is_empty(self) || pipeline_get_wait(self) ||
 * strlen(result)>0
 */
