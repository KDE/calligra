
project(kugar)
include_directories( 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/ 
	${CMAKE_SOURCE_DIR}/libs/ 
	${KOFFICECORE_INCLUDES})

set(libkudesignercommon_SRCS 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/band.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/box.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/calcfield.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/canvas.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/command.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/commdefs.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/detailbase.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/detail.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/detailfooter.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/detailheader.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/field.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/kugartemplate.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/label.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/line.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/pagefooter.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/pageheader.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/propertyserializer.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/reportfooter.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/reportheader.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/reportitem.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/section.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/specialfield.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/structurewidget.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/view.cpp 
	${CMAKE_SOURCE_DIR}/kugar/kudesigner_lib/plugin.cpp
)
 

add_subdirectory( lib )
add_subdirectory( part )
add_subdirectory( samples )
add_subdirectory( kudesigner_lib )
add_subdirectory( kudesigner )




########### install files ###############


kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )
kde4_install_icons( ${ICON_INSTALL_DIR}  crystalsvg )




#original Makefile.am contents follow:

##SUBDIRS = lib part shell samples kudesigner
#SUBDIRS = lib part samples kudesigner_lib kudesigner
#
#KDE_ICON = kugar kugardata
#
#messages: 
#	$(EXTRACTRC) lib/*.ui part/*.ui kudesigner/*.ui */*.rc > rc.cpp
#	$(XGETTEXT) -ktranslate lib/*.cpp part/*.cpp kudesigner/*.cpp kudesigner_lib/*.cpp kudesigner_lib/*.h rc.cpp -o $(podir)/kugar.pot
