#
# Robocar City Emulator and Robocar World Championship
# Justine - this is a rapid prototype for development of Robocar City Emulator
# Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

AC_INIT([justine], [0.0.17], [nbatfai@gmail.com])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE
AC_CHECK_LIB(pthread, pthread_create)
PKG_CHECK_MODULES([protobuf], [protobuf])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CXX

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(justine, Doxyfile, doc)

AC_LANG([C++])
CXXFLAGS='-Wall -std=c++1y -O3'
LDFLAGS='-lboost_system -lpthread -lboost_date_time -lboost_filesystem'
AX_CXX_COMPILE_STDCXX_11()
AC_PROG_LIBTOOL
BOOST_REQUIRE([1.46.0])
BOOST_GRAPH
BOOST_SYSTEM
BOOST_PROGRAM_OPTIONS
BOOST_THREAD
BOOST_ASIO
BOOST_DATE_TIME
#BOOST_LOG

CXXFLAGS="$AM_CXXFLAGS $CXXFLAGS"

AC_CHECK_HEADERS([osmium/osm/relation.hpp osmium/io/any_input.hpp],, 
  [please install the libosmium header-only library],
  [[
#include <osmium/osm/relation.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
    ]])

LIBS="-lbz2 $LIBS"

AC_ARG_WITH([bz2],, 
[AC_MSG_ERROR([please install the bz2 library])])

LIBS="-lz $LIBS"

AC_ARG_WITH([z],, 
[AC_MSG_ERROR([please install the z library])])

LIBS="-losmpbf $LIBS"

AC_ARG_WITH([osmpbf],, 
  [AC_MSG_ERROR([please install the osmpbf library])])

LIBS="-lprotobuf-lite $LIBS"

LIBS="-lexpat $LIBS"

AC_ARG_WITH([expat],, 
[AC_MSG_ERROR([please install the expat library])])

#save_LIBS="$LIBS"
#LIBS=''
#AC_SEARCH_LIBS([shm_open], [rt])
#SHM_LIBS="$LIBS"
#AC_SUBST(SHM_LIBS)
#LIBS="$save_LIBS"

SHM_OPEN_LIBS=""
save_LIBS="$LIBS"
AC_SEARCH_LIBS([shm_open], [rt], [SHM_OPEN_LIBS="$LIBS"])
AC_CHECK_FUNCS([shm_open])
LIBS="$save_LIBS"
AC_SUBST(SHM_OPEN_LIBS)

AM_PROG_LEX
AC_SUBST([FLEX])
AC_SUBST([FLEXFLAGS])
AC_SUBST([AM_FLEXFLAGS])

AC_CHECK_PROG([FLEX],[flex],flex, AC_MSG_ERROR([please install the flex package]))
AM_FLEXFLAGS="-olex.yy.c"

if test -z "$FLEX" ; then
AC_MSG_ERROR([please install the flex package]);
fi

AC_SUBST([LIBS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([
 src/Makefile 
 ])
AC_OUTPUT