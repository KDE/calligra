include_directories( ${CMAKE_SOURCE_DIR}/libs/guiutils ${CMAKE_SOURCE_DIR}/src/backend/common ${KDE4_INCLUDES}  )

set(pgzrptwrtembed_LIB_SRCS
	detailgroupsectiondialog.cpp
	reportentities.cpp
	reportrectentity.cpp
	reportentitylabel.cpp
	reportentityfield.cpp
	reportentitytext.cpp
	reportentityline.cpp
	reportentitybarcode.cpp
	reportentityimage.cpp
	reportsection.cpp
	reportsectiondetailgroup.cpp
	reportsectiondetail.cpp
	reportscene.cpp
	reportsceneview.cpp
	reportdesigner.cpp
	sectioneditor.cpp
	3of9paint.cpp
	ext3of9paint.cpp
	codeeanpaint.cpp
	code128paint.cpp
	)

kde4_add_ui_files(pgzrptwrtembed_LIB_SRCS detailgroupsectiondialog.ui sectioneditor.ui )

kde4_add_library(pgzrptwrtembed SHARED ${pgzrptwrtembed_LIB_SRCS})

target_link_libraries(pgzrptwrtembed pgzrptcommon ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QT3SUPPORT_LIBRARY} kexidb kexicore keximain koproperty koguiutils komain)

set_target_properties(pgzrptwrtembed PROPERTIES VERSION 0.0.1 SOVERSION 0 )

install(TARGETS pgzrptwrtembed  DESTINATION ${LIB_INSTALL_DIR})

