#
# Kexi Web Forms build script
# 

include_directories(
    ${CMAKE_SOURCE_DIR}/kexi/3rdparty/shttpd/src
    ${GOOGLE_CTEMPLATE_INCLUDE_DIR}
    ${PionNet_INCLUDE_DIR})

# Workaround, exceptions are needed for pion-net
# FIXME: This won't work for other compilers
add_definitions(-fexceptions)


# Alternatives
# - ExtJS-based view: a web2.0-ish approach to web forms
#   NOTE: ExtJS is dual licensed, we are using the copy of ExtJS licensed under
#   *GPLv3* License. You can't use the KWebforms with this view in a commercial
#   environment without paying the license, just like Qt.
#   This experimental view is disabled by default
# - Standard HTML based view: this is the standard and most stable view, built by
#   default.
if (DEFINED WITH_EXTJS_WEBFORMS)
  set(webforms_controller_SRCS controller/extjs/Controller.cpp)
  set(webforms_view_SRCS
    view/extjs/Objects.cpp)
  add_definitions(-DEXTJS_VIEW)
  message(STATUS "Kexi Web Forms: enabled Extjs view")
else (DEFINED WITH_EXTJS_WEBFORMS)
  set(webforms_controller_SRCS controller/Controller.cpp)
  set(webforms_view_SRCS
    view/View.cpp
    view/Index.cpp
    view/Create.cpp
    view/Read.cpp
    view/Update.cpp
    view/Delete.cpp
    view/Query.cpp
    view/TemplateProvider.cpp)
  message(STATUS "Kexi Web Forms: enabled standard view")
endif (DEFINED WITH_EXTJS_WEBFORMS)

kde4_add_executable(kwebforms NOGUI
# Other
  ${webforms_controller_SRCS}
  ${webforms_view_SRCS}
# Model
  model/DataProvider.cpp
  model/Database.cpp
# Other services
  FileService.cpp
  BlobService.cpp
# Authentication framework
  auth/Authenticator.cpp
  auth/User.cpp
# Main
  main.cpp)

target_link_libraries(kwebforms
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${GOOGLE_CTEMPLATE_THREAD_LIB}
    ${PionNet_LIBRARY}
    kexidb
    kexicore
    keximain)

install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
