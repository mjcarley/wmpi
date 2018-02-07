AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir) -DG_LOG_DOMAIN=\"WMPI\"

CC=@MPICC@

lib_LTLIBRARIES = libwmpi_mpi.la

libwmpi_mpi_la_SOURCES = ../src/wmpi.c
libwmpi_mpi_la_CPPFLAGS = -DWMPI_USE_MPI
