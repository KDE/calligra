add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44024)

include_directories( 
  ${CMAKE_SOURCE_DIR}/kexi 
  ${CMAKE_CURRENT_SOURCE_DIR}/completer
  ${KDE4_INCLUDES}
)

set(kexiutils_LIB_SRCS 
  identifier.cpp 
  validator.cpp 
  utils.cpp 
  styleproxy.cpp 
  longlongvalidator.cpp 
  InternalPropertyMap.cpp
  SmallToolButton.cpp 
  KexiCommandLinkButton.cpp
  FlowLayout.cpp
  transliteration_table.cpp
  kmessagewidget.cpp 
  KexiContextMessage.cpp
  KexiTitleLabel.cpp
  KexiLinkWidget.cpp
  KexiAssistantPage.cpp
  KexiAssistantWidget.cpp
  KexiAnimatedLayout.cpp
  KexiCategorizedView.cpp
  
  completer/qcompleter.cpp
)

IF (KEXI_MOBILE)

ELSE (KEXI_MOBILE)
  LIST(APPEND kexiutils_LIB_SRCS
    debuggui.cpp 
  )
ENDIF (KEXI_MOBILE)

kde4_add_library(kexiutils SHARED ${kexiutils_LIB_SRCS})

target_link_libraries(kexiutils  ${KDE4_KIO_LIBS} )
target_link_libraries(kexiutils  LINK_INTERFACE_LIBRARIES ${KDE4_KIO_LIBS})

IF (KEXI_MOBILE)
  
ELSE (KEXI_MOBILE)
  target_link_libraries(kexiutils  ${KDE4_KDE3SUPPORT_LIBS} )
  target_link_libraries(kexiutils  LINK_INTERFACE_LIBRARIES ${KDE4_KDE3SUPPORT_LIBS})
ENDIF (KEXI_MOBILE)

set_target_properties(kexiutils PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)

install(TARGETS kexiutils  ${INSTALL_TARGETS_DEFAULT_ARGS})

if(FALSE) # TODO: install when we move to independent place
    install( FILES  tristate.h identifier.h validator.h utils.h kexiutils_export.h kexiutils_global.h
    styleproxy.h longlongvalidator.h InternalPropertyMap.h 
    SmallToolButton.h FlowLayout.h
    kmessagewidget.h KexiContextMessage.h KexiTitleLabel.h KexiAssistantPage.h 
    KexiAssistantWidget.h KexiAnimatedLayout.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kexiutils COMPONENT Devel)
endif(FALSE)
