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
  GQueue *argumentos;
  char *redir_out;
  char *redir_in;
};

/*
 * Nuevo `scommand', sin comandos o argumentos y los redirectores vacíos
 *   Returns: nuevo comando simple sin ninguna cadena y redirectores vacíos.
 * Ensures: result != NULL && scommand_is_empty (result) &&
 *  scommand_get_redir_in (result) == NULL &&
 *  scommand_get_redir_out (result) == NULL
 */
scommand scommand_new(void) { // NOTA: ALOCA MEMORIA
  scommand result = malloc(sizeof(*result));
  assert(result != NULL);

  result->argumentos = g_queue_new();
  result->redir_in = NULL;
  result->redir_out = NULL;

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
scommand scommand_destroy(scommand self) {
  assert(self != NULL);
  g_queue_free_full(self->argumentos, g_free);
  self->argumentos = NULL;
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
void scommand_push_back(scommand self, char *argument) {
  assert(self != NULL);
  assert(argument != NULL);
  g_queue_push_tail(self->argumentos, g_strdup(argument));
  assert(!scommand_is_empty(self));
}

/*
 * Quita la cadena de adelante de la secuencia de cadenas.
 *   self: comando simple al cual sacarle la cadena del frente.
 * Requires: self!=NULL && !scommand_is_empty(self)
 */
void scommand_pop_front(scommand self) {
  assert(self != NULL && !scommand_is_empty(self));
  gboolean b =
      g_queue_remove(self->argumentos, g_queue_peek_head(self->argumentos));
  assert(b);
}

/*
 * Define la redirección de entrada (salida).
 *   self: comando simple al cual establecer la redirección de entrada (salida).
 *   filename: cadena con el nombre del archivo de la redirección
 *     o NULL si no se quiere redirección. El TAD se apropia de la referencia.
 * Requires: self!=NULL
 */
void scommand_set_redir_in(scommand self, char *filename) {
  assert(self != NULL);
  self->redir_in = filename;
}
void scommand_set_redir_out(scommand self, char *filename) {
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
bool scommand_is_empty(const scommand self) {
  assert(self != NULL);
  // Esto no es romper abstraccion no? Yo se que en la estructura de la gqueue hay un campo length
  return self->argumentos->length == 0u;
}

/*
 * Da la longitud de la secuencia cadenas que contiene el comando simple.
 *   self: comando simple a medir.
 *   Returns: largo del comando simple.
 * Requires: self!=NULL
 * Ensures: (scommand_length(self)==0) == scommand_is_empty(self)
 *
 */
unsigned int scommand_length(const scommand self) {
  assert(self != NULL);
  // Uso self -> length en vez de scommand_length(self)
  // para evitar infinita recursion
  assert((self->argumentos->length == 0) == scommand_is_empty(self));
  return self->argumentos->length;
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
char *scommand_front(const scommand self) {
  assert(self != NULL);
  assert(!scommand_is_empty(self));
  char *result = g_queue_peek_head(self->argumentos);
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
char *scommand_get_redir_in(const scommand self) {
  assert(self != NULL);
  return self->redir_in;
}

char *scommand_get_redir_out(const scommand self) {
  assert(self != NULL);
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
char *scommand_to_string(const scommand self) {
  assert(self != NULL);
  // Creo un GString vacio
  GString *gstr = g_string_new(NULL);
  // Itero sobre self->argumentos y voy agregando los comandos al string
  // sin romper la fucking abstraccion
  for (uint i = 0; i < g_queue_get_length(self->argumentos); i++) {
    g_string_append(gstr, g_queue_peek_nth(self->argumentos, i));
    if (i != g_queue_get_length(self->argumentos) - 1) {
      g_string_append_c(gstr, ' ');
    }
  }
  // Le meto las redirecciones si existen
  if (self->redir_in != NULL) {
    g_string_append(gstr, " < ");
    g_string_append(gstr, self->redir_in);
  }
  if (self->redir_out != NULL) {
    g_string_append(gstr, " > ");
    g_string_append(gstr, self->redir_out);
  }
  // Libera la memoria de la estructura del GString gstr
  // y me devuelve la data adentro como char*
  char *result = g_string_free(gstr, FALSE);
  assert(scommand_is_empty(self) || scommand_get_redir_in(self) == NULL ||
         scommand_get_redir_out(self) == NULL || strlen(result) > 0);
  return result;
}

/*
 * ZONA DE PIPELINE
 */

struct pipeline_s {
  GQueue *scomandos;
  bool esta_en_primer_plano;
  size_t length;
};

/*
 * Nuevo `pipeline', sin comandos simples y establecido para que espere.
 *   Returns: nuevo pipeline sin comandos simples y que espera.
 * Ensures: result != NULL
 *  && pipeline_is_empty(result)
 *  && pipeline_get_wait(result)
 */
pipeline pipeline_new(void) {
  pipeline result = malloc(sizeof(*result));
  result->scomandos = g_queue_new();
  result->esta_en_primer_plano = false;
  result->length = 0u;
  return result;
}

/*
 * Destruye `self'.
 *   self: tubería a destruir.
 * Requires: self != NULL
 * Ensures: result == NULL <-- No deberia ser self == NULL?
 */

// Wrapper para castear scommand_destroy a GDestroyNotify
static void scommand_destroy_wrapper(void *data) {
  scommand_destroy((scommand)data);
}

// NOTA: Si a alguien se le ocurre otra forma sin romper la abstraccion aca
// cambielo
pipeline pipeline_destroy(pipeline self) {
  assert(self != NULL);
  g_queue_free_full(self->scomandos, (GDestroyNotify)scommand_destroy_wrapper);
  free(self);
  return NULL;
}

/* Modificadores */

/*
 * Agrega por detrás un comando simple a la secuencia.
 *   self: pipeline al cual agregarle el comando simple.
 *   sc: comando simple a agregar. El TAD se apropia del comando.
 * Requires: self!=NULL && sc!=NULL
 * Ensures: !pipeline_is_empty(self)
 */
void pipeline_push_back(pipeline self, scommand sc) {
  assert(self != NULL);
  assert(sc != NULL);
  g_queue_push_tail(self->scomandos, sc);
  self->length++;
  assert(!pipeline_is_empty(self));
}

/*
 * Quita el comando simple de adelante de la secuencia.
 *   self: pipeline al cual sacarle el comando simple del frente.
 *      Destruye el comando extraido.
 * Requires: self!=NULL && !pipeline_is_empty(self)
 */
void pipeline_pop_front(pipeline self) {
  assert(self != NULL);
  assert(!pipeline_is_empty(self));
  // g_queue_pop_head popea el primer elemento y lo devuelve como data
  scommand killme = g_queue_pop_head(self->scomandos);
  scommand_destroy(killme);
  self->length--;
}

/*
 * Define si el pipeline tiene que esperar o no.
 *   self: pipeline que quiere ser establecido en su atributo de espera.
 * Requires: self!=NULL
 */
void pipeline_set_wait(pipeline self, const bool w) {
  assert(self != NULL);
  self->esta_en_primer_plano = w;
}

/* Proyectores */

/*
 * Indica si la secuencia de comandos simples tiene longitud 0.
 *   self: pipeline a decidir si está vacío.
 *   Returns: ¿Está vacío de comandos simples el pipeline?
 * Requires: self!=NULL
 */
bool pipeline_is_empty(const pipeline self) {
  assert(self != NULL);
  return self->length == 0u;
}

/*
 * Da la longitud de la secuencia de comandos simples.
 *   self: pipeline a medir.
 *   Returns: largo del pipeline.
 * Requires: self!=NULL
 * Ensures: (pipeline_length(self)==0) == pipeline_is_empty(self)
 *
 */
unsigned int pipeline_length(const pipeline self) {
  assert(self != NULL);
  // Uso self->length en vez de pipeline_length para evitar recursion infinita
  assert((self->length == 0) == pipeline_is_empty(self));
  return self->length;
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
scommand pipeline_front(const pipeline self) {
  assert(self != NULL);
  assert(!pipeline_is_empty(self));
  scommand result = g_queue_peek_head(self->scomandos);
  assert(result != NULL);
  return result;
}

/*
 * Consulta si el pipeline tiene que esperar o no.
 *   self: pipeline a decidir si hay que esperar.
 *   Returns: ¿Hay que esperar en el pipeline self?
 * Requires: self!=NULL
 */
bool pipeline_get_wait(const pipeline self) {
  assert(self != NULL);
  return self->esta_en_primer_plano;
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
char *pipeline_to_string(const pipeline self) {
  assert(self != NULL);
  // Creo un GString vacio
  GString *gstr = g_string_new(NULL);
  // Itero sobre self->scomandos y voy agregando los comandos al string
  // sin romper la fucking abstraccion
  for (guint i = 0; i < g_queue_get_length(self->scomandos); i++) {
    // añado la representacion del scomando en formato de string
    g_string_append(gstr, scommand_to_string(g_queue_peek_nth(self->scomandos, i)));
    if (i != g_queue_get_length(self->scomandos) - 1) {
      g_string_append(gstr, " | ");
    }
  }
  // Le meto un & si hay que esperar a que la pipeline termine (primer plano)
  if (self->esta_en_primer_plano) {
    g_string_append(gstr, " &");
  }
  // Libera la memoria de la estructura del GString gstr
  // y me devuelve la data adentro como char*
  char *result = g_string_free(gstr, FALSE);
  assert(pipeline_is_empty(self) || pipeline_get_wait(self) ||
         strlen(result) > 0);
  return result;
}
