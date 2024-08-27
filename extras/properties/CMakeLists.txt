kcoreaddons_add_plugin(calligradocinfopropspage INSTALL_NAMESPACE "kf6/propertiesdialog" SOURCES KoDocInfoPropsFactory.cpp)


target_link_libraries(calligradocinfopropspage  KF6::KIOWidgets kowidgets)
