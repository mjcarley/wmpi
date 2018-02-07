AM_CPPFLAGS = -I$(top_srcdir) -I$(includedir) -DG_LOG_DOMAIN=\"WMPI\"

bin_SCRIPTS=wmpi-config

BUILT_SOURCES= \
	wmpi-config

wmpi-config: wmpi-config.in

include_HEADERS = \
	wmpi.h 

lib_LTLIBRARIES = libwmpi_s.la 

libwmpi_s_la_SOURCES = \
	wmpi.c
	wmpi.h
