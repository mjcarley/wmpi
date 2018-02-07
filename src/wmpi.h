#ifndef _WMPI_WRAPPER_H_INCLUDED_
#define _WMPI_WRAPPER_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <glib.h>
#include <unistd.h>
#include <stdio.h>

#if 0
#define WMPI_MODE "serial"

#ifdef WMPI_USE_MPI

#include "mpi.h"
#define WMPI_MODE "MPI"

#endif /*WMPI_USE_MPI*/

#endif

extern const gchar *WMPI_MODE ;

#ifndef MPI_ANY
#define MPI_ANY -1
#endif /*MPI_ANY*/

void wmpi_log_func(const gchar *log_domain,
		  GLogLevelFlags log_level,
		  const gchar *message,
		  gpointer user_data) ;
gint wmpi_initialize(gint *argc, gchar ***argv) ;
  gint wmpi_version_mpi(gint *version, gint *subversion) ;
gint wmpi_shutdown(void) ;
guint wmpi_rank(void) ;
guint wmpi_process_number(void) ;
gint wmpi_pause(void) ;
gint wmpi_split_range(guint i, guint j, guint *pmin, guint *pmax) ;

gint wmpi_log_status_set(guint rank, gboolean status) ;
gboolean wmpi_log_status_check(guint rank) ;

gint wmpi_logging_init(FILE *f, gchar *p, 
		       GLogLevelFlags log_level,
		       gpointer exit_func) ;

gint wmpi_sum_int(gint *a, gint *b, gint n) ;
gint wmpi_sum_uint(guint *a, guint *b, gint n) ;
gint wmpi_sum_float(gfloat *a, gfloat *b, gint n) ;
gint wmpi_sum_double(gdouble *a, gdouble *b, gint n) ;

gint wmpi_sum_all_int(gint *a, gint *b, gint n) ;
gint wmpi_sum_all_uint(guint *a, guint *b, gint n) ;
gint wmpi_sum_all_float(gfloat *a, gfloat *b, gint n) ;
gint wmpi_sum_all_double(gdouble *a, gdouble *b, gint n) ;

/*   gint wmpi_sum_in_place_double(gdouble *x, gint n) ; */

gint wmpi_checkpoint(guint c) ;

gint wmpi_recv_int(gint *i, gint n, gint tag, gint sender) ;
gint wmpi_send_int(gint *i, gint n, gint tag, gint recv) ;
gint wmpi_recv_uint(guint *i, gint n, gint tag, gint sender) ;
gint wmpi_send_uint(guint *i, gint n, gint tag, gint recv) ;
gint wmpi_recv_float(gfloat *i, gint n, gint tag, gint sender) ;
gint wmpi_send_float(gfloat *i, gint n, gint tag, gint recv) ;
gint wmpi_recv_double(gdouble *i, gint n, gint tag, gint sender) ;
gint wmpi_send_double(gdouble *i, gint n, gint tag, gint recv) ;

gint wmpi_broadcast_int(gint *data, gint n, gint proc) ;
gint wmpi_broadcast_uint(guint *data, gint n, gint proc) ;
gint wmpi_broadcast_float(gfloat *data, gint n, gint proc) ;
gint wmpi_broadcast_double(gdouble *data, gint n, gint proc) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*_WMPI_WRAPPER_H_INCLUDED_*/

