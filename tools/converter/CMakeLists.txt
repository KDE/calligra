




########### next target ###############

set(koconverter_SRCS koconverter.cpp )

kde4_automoc(${koconverter_SRCS})

kde4_add_executable(koconverter ${koconverter_SRCS})

target_link_libraries(koconverter  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(/bin koconverter )


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
#
#bin_PROGRAMS = koconverter
#koconverter_SOURCES = koconverter.cpp
#koconverter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#koconverter_LDADD = $(top_builddir)/lib/kofficecore/libkofficecore.la
#
#METASOURCES = AUTO
#
#messages:
#	$(XGETTEXT) *.cpp -o $(podir)/koconverter.pot
