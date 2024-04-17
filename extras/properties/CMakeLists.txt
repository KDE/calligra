kcoreaddons_add_plugin(calligradocinfopropspage INSTALL_NAMESPACE "kf5/propertiesdialog" SOURCES KoDocInfoPropsFactory.cpp)


target_link_libraries(calligradocinfopropspage  KF5::KIOWidgets kowidgets)
