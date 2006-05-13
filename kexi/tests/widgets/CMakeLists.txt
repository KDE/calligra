

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidbdrivercombotest_SRCS kexidbdrivercombotest.cpp )

kde4_automoc(${kexidbdrivercombotest_SRCS})

kde4_add_executable(kexidbdrivercombotest ${kexidbdrivercombotest_SRCS})

target_link_libraries(kexidbdrivercombotest  ${KDE4_KDECORE_LIBS} kexidb kexiextendedwidgets )


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#noinst_PROGRAMS = kexidbdrivercombotest
#
#INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
#
#kexidbdrivercombotest_SOURCES = kexidbdrivercombotest.cpp
#kexidbdrivercombotest_LDADD = $(LIB_QT) $(LIB_KDECORE) \
# $(top_builddir)/kexi/kexidb/libkexidb.la \
# $(top_builddir)/kexi/widget/libkexiextendedwidgets.la
#kexidbdrivercombotest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#METASOURCES = AUTO
#
