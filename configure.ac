AC_INIT(wmpi, 1.2)
AC_CONFIG_SRCDIR([src/wmpi.h])

AC_CONFIG_MACRO_DIRS([m4])

# Making releases:
#   WMPI_MICRO_VERSION += 1;
#   WMPI_INTERFACE_AGE += 1;
#   WMPI_BINARY_AGE += 1;
# if any functions have been added, set WMPI_INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set WMPI_BINARY_AGE and WMPI_INTERFACE_AGE to 0.
#
WMPI_MAJOR_VERSION=0
WMPI_MINOR_VERSION=1
WMPI_MICRO_VERSION=2
WMPI_INTERFACE_AGE=0
WMPI_BINARY_AGE=0
WMPI_VERSION=$WMPI_MAJOR_VERSION.$WMPI_MINOR_VERSION.$WMPI_MICRO_VERSION

AC_SUBST(WMPI_MAJOR_VERSION)
AC_SUBST(WMPI_MINOR_VERSION)
AC_SUBST(WMPI_MICRO_VERSION)
AC_SUBST(WMPI_VERSION)

# libtool versioning
LT_RELEASE=$WMPI_MAJOR_VERSION.$WMPI_MINOR_VERSION
LT_CURRENT=`expr $WMPI_MICRO_VERSION - $WMPI_INTERFACE_AGE`
LT_REVISION=$WMPI_INTERFACE_AGE
LT_AGE=`expr $WMPI_BINARY_AGE - $WMPI_INTERFACE_AGE`
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

# For automake.
VERSION=$WMPI_VERSION
PACKAGE=wmpi


##AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define, [subdir-objects])
AM_INIT_AUTOMAKE([subdir-objects])

# Specify a configuration file
AM_CONFIG_HEADER(config.h)

AC_DEFINE_UNQUOTED(WMPI_MAJOR_VERSION, $WMPI_MAJOR_VERSION, [Major version])
AC_DEFINE_UNQUOTED(WMPI_MINOR_VERSION, $WMPI_MINOR_VERSION, [Minor version])
AC_DEFINE_UNQUOTED(WMPI_MICRO_VERSION, $WMPI_MICRO_VERSION, [Micro version])
AC_DEFINE_UNQUOTED(WMPI_INTERFACE_AGE, $WMPI_INTERFACE_AGE, [Interface age])
AC_DEFINE_UNQUOTED(WMPI_BINARY_AGE, $WMPI_BINARY_AGE, [Binary age])

dnl Initialize libtool
AM_PROG_LIBTOOL

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

AC_PROG_CC

if test x$GCC = xyes ; then
  CFLAGS="$CFLAGS -Wall -Werror-implicit-function-declaration -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations"
fi

AC_PROG_AWK
AC_ISC_POSIX
AC_HEADER_STDC

# taken from Stephane Popinet's GTS configure.in

AM_PATH_GLIB_2_0(2.4.0, has_glib2=yes, has_glib2=no, gthread gmodule)
if test x$has_glib2 = xyes; then
  glib_cflags=`$PKG_CONFIG glib-2.0 --cflags`
  glib_thread_cflags=`$PKG_CONFIG glib-2.0 --cflags gthread-2.0`
  glib_module_cflags=`$PKG_CONFIG glib-2.0 --cflags gmodule-2.0`
  glib_libs=`$PKG_CONFIG glib-2.0 --libs`
  glib_thread_libs=`$PKG_CONFIG glib-2.0 --libs gthread-2.0`
  glib_module_libs=`$PKG_CONFIG glib-2.0 --libs gmodule-2.0`
  GLIB_LIBS="$glib_libs"
  GLIB_DEPLIBS="$glib_libs"
else
  if test x$with_glib = xyes ; then
    AC_MSG_ERROR([
*** Directory must be specified for --with-glib])
  fi

  if test x$with_glib = x ; then 
    # Look for separately installed glib

    GLIB_REQUIRED_VERSION=1.2.8
    AM_PATH_GLIB($GLIB_REQUIRED_VERSION,,
      AC_MSG_ERROR([
*** GLIB $GLIB_REQUIRED_VERSION or better is required. The latest version of GLIB
*** is always available from ftp://ftp.gtk.org/.]),
      gmodule gthread)

    glib_cflags=`$GLIB_CONFIG --cflags`
    glib_thread_cflags=`$GLIB_CONFIG --cflags gthread`
    glib_module_cflags=`$GLIB_CONFIG --cflags gmodule`
    glib_libs=`$GLIB_CONFIG --libs`
    glib_thread_libs=`$GLIB_CONFIG --libs gthread`
    glib_module_libs=`$GLIB_CONFIG --libs gmodule`
    GLIB_LIBS="$glib_libs"
    GLIB_DEPLIBS="$glib_libs"
  else
    # Use uninstalled glib (assume they got the version right)

    GLIB_CONFIG=$with_glib/glib-config
    if test -x $GLIB_CONFIG ; then 
      :
    else
      AC_MSG_ERROR([GLIB directory ($with_glib) not present or not configured])
    fi

    # For use in wmpi-config
    glib_cflags=`$GLIB_CONFIG --cflags`
    glib_thread_cflags=`$GLIB_CONFIG --cflags gthread`
    glib_module_cflags=`$GLIB_CONFIG --cflags gmodule`
    glib_libs=`$GLIB_CONFIG --libs`
    glib_thread_libs=`$GLIB_CONFIG --libs gthread`
    glib_module_libs=`$GLIB_CONFIG --libs gmodule`

    glib_release=`$GLIB_CONFIG --version | sed 's%\\.[[0-9]]*$%%'`

    # canonicalize relative paths
    case $with_glib in 
      /*)
        glib_dir=$with_glib
        ;;
      *)
        glib_dir="\$(top_builddir)/$with_glib"
        ;;
    esac

    GLIB_CFLAGS="-I$glib_dir -I$glib_dir/gmodule"
    GLIB_LIBS="$glib_dir/libglib.la $glib_dir/gmodule.la"
    GLIB_DEPLIBS=

    AC_SUBST(GLIB_CFLAGS)
    AC_SUBST(GLIB_LIBS)
  fi
fi

AC_CHECK_PROG(mpicc_available, mpicc, yes, no)
if test x$mpicc_available = xyes ; then
  echo \#################################################################
  echo 
  echo \# mpicc found\; compiling with MPI parallel support.
  echo
  echo \#################################################################
  MPICC="mpicc"
  MPIFLAGS="-DWMPI_USE_MPI"
  USERMPIFLAGS=""
  mpi_dir="mpi"
else
  echo \#################################################################
  echo \#
  echo \# mpicc not found\; compiling serial library only
  echo \#
  echo \#################################################################
  MPICC=""
  MPIFLAGS=""
  USERMPIFLAGS=""
  mpi_dir=""
fi

AC_CHECK_PROG(mpiCC_available, mpiCC, yes, no)
if test x$mpiCC_available = xyes ; then
  echo \#################################################################
  echo 
  echo \# mpiCC found\; compiling with MPI C++ parallel support.
  echo
  echo \#################################################################
  MPICXX="mpiCC"
  MPICXXFLAGS="-DWMPI_USE_MPI"
##  USERMPIFLAGS=""
##  mpi_dir="mpi"
else
  MPICXX=""
  MPICXXFLAGS=""
fi

AC_CHECK_PROG(mpicxx_available, mpicxx, yes, no)
if test x$mpicxx_available = xyes ; then
  echo \#################################################################
  echo
  echo \# mpicxx found\; compiling with MPI C++ parallel support.
  echo
  echo \#################################################################
  MPICXX="mpicxx"
  MPICXXFLAGS="-DWMPI_USE_MPI"
fi

AC_SUBST(glib_cflags)
AC_SUBST(glib_libs)
AC_SUBST(mpicc_available)
AC_SUBST(mpiCC_available)
AC_SUBST(MPICC)
AC_SUBST(MPICXX)
AC_SUBST(MPIFLAGS)
AC_SUBST(MPICXXFLAGS)
AC_SUBST(USERMPIFLAGS)
AC_SUBST(mpi_dir)

CFLAGS="$CFLAGS $glib_cflags"
LDFLAGS="$LDFLAGS $glib_libs"

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

LIBS="$glib_libs $LIBS"

AM_CONDITIONAL(COND_MPI, [test x$mpicc_available = xyes])

AC_CONFIG_FILES([
Makefile
src/Makefile
src/wmpi-config
mpi/Makefile
])

AC_OUTPUT
