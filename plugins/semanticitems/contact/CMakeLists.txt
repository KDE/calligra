add_definitions(-DTRANSLATION_DOMAIN=\"calligra_semanticitem_contact\")

include_directories(
    ${KORDF_INCLUDES}
)
set(calligra_semanticitem_contact_LIB_SRCS
    KoContactSemanticItemFactory.cpp
    KoContactSemanticItemPlugin.cpp
    KoRdfFoaF.cpp
    KoRdfFoaFTreeWidgetItem.cpp
)

ki18n_wrap_ui( calligra_semanticitem_contact_LIB_SRCS
   KoRdfFoaFEditWidget.ui
)

add_library(calligra_semanticitem_contact MODULE ${calligra_semanticitem_contact_LIB_SRCS})

if (KF6Contacts_FOUND AND KPim6Akonadi_FOUND)
   add_definitions(-DKDEPIMLIBS_FOUND)
   target_link_libraries(calligra_semanticitem_contact
      KF6::Contacts
      KPim6::AkonadiCore
   )
endif ()

target_link_libraries(calligra_semanticitem_contact   ${KORDF_LIBS} KF6::I18n KF6::KDELibs4Support)

install(TARGETS calligra_semanticitem_contact  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/semanticitems)
