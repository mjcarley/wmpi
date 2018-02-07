/* wmpi.c
 * 
 * Copyright (C) 2006 Michael Carley
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "wmpi.h"

#ifndef WMPI_USE_MPI

const gchar *WMPI_MODE="serial" ;

#endif /*WMPI_USE_MPI*/

#ifdef WMPI_USE_MPI

const gchar *WMPI_MODE="MPI" ;

#endif /*WMPI_USE_MPI*/

#define WMPI_LOGGING_DATA_WIDTH     4
#define WMPI_LOGGING_DATA_FID       0
#define WMPI_LOGGING_DATA_PREFIX    1
#define WMPI_LOGGING_DATA_LEVEL     2
#define WMPI_LOGGING_DATA_EXIT_FUNC 3

guint _wmpi_rank ;
guint _wmpi_n_processes = 0 ;
gboolean *_wmpi_log_status ;

const gchar *wmpi_logging_string(GLogLevelFlags level) ;

#ifdef WMPI_USE_MPI

#include "mpi.h"

/**
 * @defgroup WMPI Basic WMPI functions
 *
 * @{
 * 
 */

gint wmpi_version_mpi(gint *version, gint *subversion)

{
  MPI_Get_version(version, subversion) ;

  return 0 ;
}

/** 
 * Initialization on parallel systems. If running on MPI, this calls
 * MPI_Init; on a serial run, the function does nothing. This function
 * should be called before any other WMPI call. 
 * 
 * @param argc number of command line options
 * @param argv 
 * 
 * @return 0 on success.
 */

gint wmpi_initialize(gint *argc, gchar ***argv)

{
  gint i ;

  i = MPI_Init(argc, argv) ;
  if ( i != 0 ) {
    fprintf(stderr, "%s: MPI_Init returned non-zero value %d",
	    __FUNCTION__, i) ;
    wmpi_shutdown() ;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &i) ; _wmpi_rank = (guint)i ;
  MPI_Comm_size(MPI_COMM_WORLD, &i) ; _wmpi_n_processes = (guint)i ;

  _wmpi_log_status = (gboolean *)g_malloc(_wmpi_n_processes*sizeof(gboolean)) ;
  for ( i = 1 ; i < (gint)_wmpi_n_processes ; i ++ )
    wmpi_log_status_set((guint)i, FALSE) ;
  _wmpi_log_status[0] = TRUE ;

  wmpi_logging_init(stderr, "", G_LOG_LEVEL_DEBUG, wmpi_shutdown) ;

  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
	"%s: initialized for %s (%u processes)", 
	__FUNCTION__, WMPI_MODE, _wmpi_n_processes) ;

  return 0 ;
}

/** 
 * Shut down parallel operation unless on a serial system, when
 * nothing happens.
 * 
 * @return 0 on success.
 */

gint wmpi_shutdown()

{
  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
	"%s: shutting down %s operation", __FUNCTION__, WMPI_MODE) ;
  MPI_Finalize() ;
  return 0 ;
}

gint wmpi_sum_int(gint *a, gint *b, gint n)

{
  MPI_Reduce((gpointer)b, (gpointer)a, n, MPI_INT,
	     MPI_SUM, 0, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_sum_all_int(gint *a, gint *b, gint n)

{
  MPI_Allreduce((gpointer)b, (gpointer)a, (gint)n, MPI_INT,
		MPI_SUM, MPI_COMM_WORLD) ;
  return 0 ;
}

gint wmpi_sum_uint(guint *a, guint *b, gint n)

{
  MPI_Reduce((gpointer)b, (gpointer)a, (gint)n, MPI_UNSIGNED,
	     MPI_SUM, 0, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_sum_all_uint(guint *a, guint *b, gint n)

{
  MPI_Allreduce((gpointer)b, (gpointer)a, (gint)n, MPI_UNSIGNED,
		MPI_SUM, MPI_COMM_WORLD) ;
  return 0 ;
}

gint wmpi_sum_float(gfloat *a, gfloat *b, gint n)

{
  MPI_Reduce((gpointer)b, (gpointer)a, n, MPI_FLOAT,
	     MPI_SUM, 0, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_sum_all_float(gfloat *a, gfloat *b, gint n)

{
  MPI_Allreduce((gpointer)b, (gpointer)a, (gint)n, MPI_FLOAT,
		MPI_SUM, MPI_COMM_WORLD) ;
  return 0 ;
}


gint wmpi_sum_double(gdouble *a, gdouble *b, gint n)

{
  MPI_Reduce((gpointer)b, (gpointer)a, n, MPI_DOUBLE,
	     MPI_SUM, 0, MPI_COMM_WORLD) ;

  return 0 ;
}


gint wmpi_sum_all_double(gdouble *a, gdouble *b, gint n)

{
  MPI_Allreduce((gpointer)b, (gpointer)a, (gint)n, MPI_DOUBLE,
		MPI_SUM, MPI_COMM_WORLD) ;
  return 0 ;
}

/* gint wmpi_sum_in_place_double(gdouble *x, gint n) */

/* { */
/*   MPI_Allreduce(MPI_IN_PLACE, (gpointer)x, (gint)n, MPI_DOUBLE, */
/* 		MPI_SUM, MPI_COMM_WORLD) ; */

/*   return 0 ; */
/* } */

gint wmpi_pause()

{
  MPI_Barrier(MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_send_int(gint *i, gint n, gint tag, gint recv)

{
  MPI_Send(i, n, MPI_INT, recv, tag, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_recv_int(gint *i, gint n, gint tag, gint sender)

{
  MPI_Status status ;

  MPI_Recv(i, n, MPI_INT, sender, tag, MPI_COMM_WORLD, &status) ;

  return 0 ;
}

gint wmpi_send_uint(guint *i, gint n, gint tag, gint recv)

{
  MPI_Send(i, n, MPI_UNSIGNED, recv, tag, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_recv_uint(guint *i, gint n, gint tag, gint sender)

{
  MPI_Status status ;

  MPI_Recv(i, n, MPI_UNSIGNED, sender, tag, MPI_COMM_WORLD, &status) ;

  return 0 ;
}

gint wmpi_send_double(gdouble *i, gint n, gint tag, gint recv)

{
  MPI_Send(i, n, MPI_DOUBLE, recv, tag, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_recv_double(gdouble *i, gint n, gint tag, gint sender)

{
  MPI_Status status ;

  MPI_Recv(i, n, MPI_DOUBLE, sender, tag, MPI_COMM_WORLD, &status) ;

  return 0 ;
}

gint wmpi_broadcast_double(gdouble *data, gint n, gint proc)

{
  MPI_Bcast(data, n, MPI_DOUBLE, proc, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_broadcast_int(gint *data, gint n, gint proc)

{
  MPI_Bcast(data, n, MPI_INT, proc, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_broadcast_uint(guint *data, gint n, gint proc)

{
  MPI_Bcast(data, n, MPI_UNSIGNED, proc, MPI_COMM_WORLD) ;

  return 0 ;
}


gint wmpi_send_float(gfloat *i, gint n, gint tag, gint recv)

{
  MPI_Send(i, n, MPI_FLOAT, recv, tag, MPI_COMM_WORLD) ;

  return 0 ;
}

gint wmpi_recv_float(gfloat *i, gint n, gint tag, gint sender)

{
  MPI_Status status ;

  MPI_Recv(i, n, MPI_FLOAT, sender, tag, MPI_COMM_WORLD, &status) ;

  return 0 ;
}

gint wmpi_broadcast_float(gfloat *data, gint n, gint proc)

{
  MPI_Bcast(data, n, MPI_FLOAT, proc, MPI_COMM_WORLD) ;

  return 0 ;
}
#else /*WMPI_USE_MPI*/

gint wmpi_version_mpi(gint *version, gint *subversion)

{
  *version = *subversion = 0 ;

  return 0 ;
}

gint wmpi_initialize(gint *argc, gchar ***argv)

{
  _wmpi_rank = 0 ;
  _wmpi_n_processes = 1 ;

  _wmpi_log_status = (gboolean *)g_malloc(_wmpi_n_processes*sizeof(gboolean)) ;
  *_wmpi_log_status = TRUE ;

  wmpi_logging_init(stderr, "", G_LOG_LEVEL_MESSAGE, wmpi_shutdown) ;
  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
	"%s: initialized for %s", __FUNCTION__, WMPI_MODE) ;

  return 0 ;
}

gint wmpi_shutdown()

{
  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE,
	"%s: shutting down %s operation", __FUNCTION__, WMPI_MODE) ;

  g_free(_wmpi_log_status) ;

  return 0 ;
}

gint wmpi_sum_int(gint *a, gint *b, gint n)

{
  g_memmove(a,b,n*sizeof(gint)) ; return 0 ;
}

gint wmpi_sum_all_int(gint *a, gint *b, gint n)

{
  g_memmove(a,b,n*sizeof(gint)) ; return 0 ;  
}

gint wmpi_sum_all_uint(guint *a, guint *b, gint n)

{
  g_memmove(a,b,n*sizeof(guint)) ; return 0 ;  
}

gint wmpi_sum_double(gdouble *a, gdouble *b, gint n)

{
  g_memmove(a,b,n*sizeof(gdouble)) ; return 0 ;
}

gint wmpi_sum_all_double(gdouble *a, gdouble *b, gint n)

{
  g_memmove(a,b,n*sizeof(gdouble)) ; return 0 ;
}

gint wmpi_pause()

{ return 0 ; }

gint wmpi_broadcast_int(gint *data, gint n, gint proc)

{ return 0 ; }

gint wmpi_broadcast_uint(guint *data, gint n, gint proc)

{ return 0 ; }

gint wmpi_broadcast_double(gdouble *data, gint n, gint proc)

{ return 0 ; }

gint wmpi_send_double(gdouble *i, gint n, gint tag, gint recv)
 
{ return 0 ; }

gint wmpi_recv_double(gdouble *i, gint n, gint tag, gint sender)

{ return 0 ; }

gint wmpi_send_float(gfloat *i, gint n, gint tag, gint recv)
 
{ return 0 ; }

gint wmpi_recv_float(gfloat *i, gint n, gint tag, gint sender)

{ return 0 ; }

gint wmpi_broadcast_float(gfloat *data, gint n, gint proc)

{ return 0 ; }

gint wmpi_send_int(gint *i, gint n, gint tag, gint recv)

{ return 0 ; }

gint wmpi_recv_int(gint *i, gint n, gint tag, gint sender)

{ return 0 ; }

gint wmpi_send_uint(guint *i, gint n, gint tag, gint recv)

{ return 0 ; }

gint wmpi_recv_uint(guint *i, gint n, gint tag, gint sender)

{ return 0 ; }

#endif /*WMPI_USE_MPI*/

/*These functions are the same in either case.*/

gint wmpi_log_status_set(guint rank, gboolean status)

{
  if ( rank >= _wmpi_n_processes || rank < 0 ) 
    g_error("%s: rank (%d) out of range (%d processes)",
	    __FUNCTION__, rank, _wmpi_n_processes) ;

  _wmpi_log_status[rank] = status ;

  return 0 ;
}

gboolean wmpi_log_status_check(guint rank)

{
  if ( rank >= _wmpi_n_processes || rank < 0 ) 
    g_error("%s: rank (%d) out of range (%d processes)",
	    __FUNCTION__, rank, _wmpi_n_processes) ;

  return (_wmpi_log_status[rank]) ;
}

guint wmpi_rank() 

{
  if ( _wmpi_n_processes == 0 )
    g_error("%s: wmpi not initialized, use wmpi_initialize()",
	    __FUNCTION__) ;

  return _wmpi_rank ;
}

guint wmpi_process_number() 

{
  if ( _wmpi_n_processes == 0 )
    g_error("%s: wmpi not initialized, use wmpi_initialize()",
	    __FUNCTION__) ;

  return _wmpi_n_processes ;
}

gint wmpi_split_range(guint i, guint j, guint *pmin, guint *pmax)

{
  g_debug("P%d: %s: splitting range (%u, %u)",
	  wmpi_rank(), __FUNCTION__, i, j) ;

  if ( pmin == NULL || pmax == NULL ) 
    g_error("%s: pmin and pmax may not be NULL", __FUNCTION__) ;

  *pmin = (j-i)/_wmpi_n_processes*_wmpi_rank ;
  if ( _wmpi_rank == _wmpi_n_processes - 1) *pmax = j ;
  else
    *pmax = (j-i)/_wmpi_n_processes*(_wmpi_rank+1) ;

  g_debug("P%d: %s: range (%u,%u) split to (%u, %u)",
	  wmpi_rank(), __FUNCTION__, i, j, *pmin, *pmax) ;

  return 0 ;
}

const gchar *wmpi_logging_string(GLogLevelFlags level)

{
  const gchar *strings[] = {"RECURSION", 
			    "FATAL",
			    "ERROR",
			    "CRITICAL",
			    "WARNING",
			    "MESSAGE",
			    "INFO",
			    "DEBUG"} ;

  switch (level) {
  default: g_assert_not_reached() ; break ;
  case G_LOG_FLAG_RECURSION: return strings[0] ;
  case G_LOG_FLAG_FATAL: return strings[1] ;
  case G_LOG_LEVEL_ERROR: return strings[2] ;
  case G_LOG_LEVEL_CRITICAL: return strings[3] ;
  case G_LOG_LEVEL_WARNING: return strings[4] ;
  case G_LOG_LEVEL_MESSAGE: return strings[5] ;
  case G_LOG_LEVEL_INFO: return strings[6] ;
  case G_LOG_LEVEL_DEBUG: return strings[7] ;
  }

  return NULL ;
}

static void wmpi_logging_func(const gchar *log_domain,
			      GLogLevelFlags log_level,
			      const gchar *message,
			      gpointer data[])

{
  FILE *f = (FILE *)data[WMPI_LOGGING_DATA_FID] ;
  gchar *p = (gchar *)data[WMPI_LOGGING_DATA_PREFIX] ;
  GLogLevelFlags level = *(GLogLevelFlags *)data[WMPI_LOGGING_DATA_LEVEL] ;
  gint (*exit_func)(void) ;

  exit_func = data[WMPI_LOGGING_DATA_EXIT_FUNC] ;

  if ( log_level > level ) return ;
  if ( !wmpi_log_status_check(wmpi_rank()) ) return ;

  fprintf(f, "%s%s-%s: %s\n", p, 
	  G_LOG_DOMAIN, wmpi_logging_string(level),
	  message) ;

  if ( log_level <= G_LOG_LEVEL_ERROR ) {
    if ( exit_func != NULL ) exit_func() ;
  }

  return ;
}

/** 
 * Initialize WMPI logging
 * 
 * @param f file stream for messages
 * @param p string to prepend to messages
 * @param log_level maximum logging level to handle (see g_log)
 * @param exit_func function to call if exiting on an error
 * 
 * @return 0 on success
 */

gint wmpi_logging_init(FILE *f, gchar *p, 
		      GLogLevelFlags log_level,
		      gpointer exit_func)

{
  static gpointer data[WMPI_LOGGING_DATA_WIDTH] ;
  static GLogLevelFlags level ;

  if ( f != NULL ) 
    data[WMPI_LOGGING_DATA_FID] = f ;
  else
    data[WMPI_LOGGING_DATA_FID] = stderr ;    
  if ( p != NULL ) 
    data[WMPI_LOGGING_DATA_PREFIX] = g_strdup(p) ;
  else
    data[WMPI_LOGGING_DATA_PREFIX] = g_strdup("") ;

  level = log_level ;
  data[WMPI_LOGGING_DATA_LEVEL] = &level ;    
    
  g_log_set_handler (G_LOG_DOMAIN, 
		     G_LOG_FLAG_RECURSION |
		     G_LOG_FLAG_FATAL |   
		     G_LOG_LEVEL_ERROR |
		     G_LOG_LEVEL_CRITICAL |
		     G_LOG_LEVEL_WARNING |
		     G_LOG_LEVEL_MESSAGE |
		     G_LOG_LEVEL_INFO |
		     G_LOG_LEVEL_DEBUG,
		     (GLogFunc)wmpi_logging_func, data) ;

  return 0 ;
}

/**
 * @}
 * 
 */
