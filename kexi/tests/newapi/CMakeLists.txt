

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )

# temp
#ADD_DEFINITIONS( -DNO_GUI=1 )


########### next target ###############

set(kexidbtest_SRCS main.cpp )

kde4_add_executable(kexidbtest NOGUI ${kexidbtest_SRCS})

target_link_libraries(kexidbtest  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QT3SUPPORT_LIBRARY} kexidb kexiextendedwidgets kexidatatable )


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
## unused: kexidbmysqlcursor kexidbfirebirdcursor
#
#noinst_PROGRAMS = kexidbtest
# 
## unused: kexidbmysqlcursor
#
#INCLUDES = -I$(top_srcdir)/kexi \
#  -I$(top_srcdir)/kexi/widget \
#  -I$(top_srcdir)/kexi/core \
#  $(all_includes)
#
#SUBDIRS = .
#
#METASOURCES = AUTO
#
#kexidbtest_SOURCES = main.cpp
#kexidbtest_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
# ../../kexidb/parser/libkexidbparser.la \
# $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
#kexidbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
##kexidbmysqlcursor_SOURCES = mysqlcursor.cpp
##kexidbmysqlcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la \
## ../../kexidb/parser/libkexidbparser.la
##kexidbmysqlcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
##kexidbfirebirdcursor_SOURCES = firebirdcursor.cpp
##kexidbfirebirdcursor_LDADD = $(LIB_QT) $(LIB_KDECORE) $(top_builddir)/kexi/kexidb/libkexidb.la
##kexidbfirebirdcursor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
