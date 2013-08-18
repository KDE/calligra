
set(itemviews_SRCS
  kcategorizedsortfilterproxymodel.cpp
  kcategorizedview.cpp
  kcategorydrawer.cpp
  kextendableitemdelegate.cpp
  kfilterproxysearchline.cpp
  klistwidgetsearchline.cpp
  ktreewidgetsearchline.cpp
  kwidgetitemdelegate.cpp
  kwidgetitemdelegatepool.cpp
)

add_library(ItemViews ${itemviews_SRCS})
generate_export_header(ItemViews)

# KCoreAddons dependency is required by kcategorizedsortfilterproxymodel (KStringHandler::naturalCompare)
target_link_libraries(ItemViews LINK_PUBLIC Qt5::Widgets KCoreAddons)

set_target_properties(ItemViews PROPERTIES VERSION   ${ITEMVIEWS_VERSION_STRING}
                                           SOVERSION ${ITEMVIEWS_SOVERSION}
)

install(TARGETS ItemViews EXPORT ItemViewsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  kcategorizedsortfilterproxymodel.h
  kcategorizedview.h
  kcategorydrawer.h
  kextendableitemdelegate.h
  kfilterproxysearchline.h
  klistwidgetsearchline.h
  ktreewidgetsearchline.h
  kwidgetitemdelegate.h
  ${CMAKE_CURRENT_BINARY_DIR}/itemviews_export.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
