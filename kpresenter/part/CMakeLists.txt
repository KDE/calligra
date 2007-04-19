project(kpresenter)

include_directories(${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES} ${KOPAGEAPP_INCLUDES})

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPRESENTER_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

### kpresenterprivate ###
set( kpresenterprivate_LIB_SRCS
     KPrFactory.cpp
     KPrDocument.cpp
     KPrView.cpp
     KPrViewModePresentation.cpp
   )

kde4_automoc(${kpresenterprivate_LIB_SRCS})
#kde4_add_ui_files(kpresenterprivate_LIB_SRCS xxxx.ui)

kde4_add_library(kpresenterprivate SHARED ${kpresenterprivate_LIB_SRCS})

target_link_libraries(kpresenterprivate ${KDE4_KDECORE_LIBS} kofficeui flake kotext kopageapp)

set_target_properties(kpresenterprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kpresenterprivate  DESTINATION ${LIB_INSTALL_DIR})

### kpresenterpart ###
set(kpresenterpart_PART_SRCS KPrFactoryInit.cpp )

kde4_automoc(${kpresenterpart_PART_SRCS})

kde4_add_plugin(kpresenterpart WITH_PREFIX ${kpresenterpart_PART_SRCS})



target_link_libraries(kpresenterpart  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install(TARGETS kpresenterpart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit kpresenter ###
set(kpresenter_KDEINIT_SRCS main.cpp )

kde4_automoc(${kpresenter_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kpresenter ${kpresenter_KDEINIT_SRCS})

target_link_libraries(kdeinit_kpresenter  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kpresenter DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries(kpresenter kdeinit_kpresenter)
install(TARGETS kpresenter DESTINATION ${BIN_INSTALL_DIR})

### desktop files ###
install( FILES kpresenterpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kpresenter.desktop DESTINATION ${XDG_APPS_DIR})

### GUI files ###
install( FILES kpresenter.rc kpresenter_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kpresenter)
